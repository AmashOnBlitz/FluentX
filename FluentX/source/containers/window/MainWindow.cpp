#include "core/pch.h"
#include "containers/window/MainWindow.h"
#include "core/CustomWindowMessages.h"
#include "core/WindowsConstants.h"
#include "core/macroFuncs.h"
#include "core/utility.h"
#include "Windows.h"
#include "tchar.h"

NAMESPACE_FLUENTX::MainWindow::MainWindow()
{
	mOnClose = [](std::string) {};
}

NAMESPACE_FLUENTX::MainWindow::~MainWindow()
{
	if (mOnClose) mOnClose = nullptr;
}

bool NAMESPACE_FLUENTX::MainWindow::Init(
	std::string windowName,
	int width,
	int height,
	int xPos,
	int yPos,
	MainWindow* parent,
	MainWindowStyle style,
	MainWindowTransitionSet transitions
)
{
	this->mMainWndTransSet = transitions;
	this->mStyle = style;
	static int windowGenCount = 0;
	mWindowName = std::wstring(windowName.begin(), windowName.end()); 
	mClassName = L"Class_MAINWINDOW_" + mWindowName + std::to_wstring(++windowGenCount);
	WNDCLASSEX wndClassEx{};
	wndClassEx.cbSize = sizeof(wndClassEx);
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	//wndClassEx.hbrBackground = NULL;
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject((parent)? BLACK_BRUSH : GRAY_BRUSH);
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClassEx.hInstance = WindowsConstants::getHinstance();
	wndClassEx.lpfnWndProc = fnMainWinProcNavigator;
	wndClassEx.lpszClassName = mClassName.c_str();
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	this->getWndContext().wndClassEx = wndClassEx;
	if (!RegisterClassEx(&this->getWndContext().wndClassEx)) {
		DWORD errorCode = GetLastError();
		std::string err = "Cannot Create Window Class Of Name \"" +
			WStringToString(mWindowName) +
			"\"!\nError Code: " + std::to_string(errorCode) +
			" In Window Class Registration";
		this->setLastErr(err);
		FLUENTX_THROW_ERROR(err);
		return false;
	}
	HWND parentHwnd = parent ? parent->getWndContext().hWnd : NULL;
	DWORD dwStyle = ConvToWin32WndStyle_Creation(style.creation);
	if (parent)
	{
		dwStyle &= ~(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
		dwStyle |= WS_CHILD | WS_VISIBLE;
	}

	HWND hWnd = CreateWindowEx(
		ConvToWin32WndExStyle_Creation(style.creation),
		mClassName.c_str(),
		mWindowName.c_str(),
		dwStyle,
		xPos,
		yPos,
		width,
		height,
		parentHwnd,
		NULL,
		WindowsConstants::getHinstance(),
		this
		);
	if (hWnd == INVALID_HANDLE_VALUE || hWnd == NULL) {
		DWORD errorCode = GetLastError();
		std::string err = "Cannot Create Window  Of Name \"" +
			WStringToString(mWindowName) +
			"\"!\nError Code: " + std::to_string(errorCode) + " In Create Window";
		this->setLastErr(err);
		FLUENTX_THROW_ERROR(err);
		return false;
	}
	this->ApplyUIFlags(hWnd, mStyle.ui);
	ApplyBehaviorFlags(hWnd, mStyle.behavior);
	this->getWndContext().hWnd = hWnd;
	return true;
}

//
//void NAMESPACE_FLUENTX::MainWindow::setParent(MainWindow& parent)
//{
//	this->getWndContext().hParent = parent.getWndContext().hWnd;
//	::SetParent(this->getWndContext().hWnd, this->getWndContext().hParent);
//	LONG style = GetWindowLong(this->getWndContext().hWnd, GWL_STYLE);
//	style = (style & ~WS_POPUP) | WS_CHILD;
//	SetWindowLong(this->getWndContext().hWnd, GWL_STYLE, style);
//}

LRESULT NAMESPACE_FLUENTX::MainWindow::fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HMENU hMenu = nullptr;

	switch (msg)
	{
	case WM_CREATE:
	case WM_FLUENTX_REBUILD_MENU:
	{
		if (!mMenuBar) return 1;
		AllVectMenuItems.clear();
		hMenu = CreateMenu();
		int iMenuID = 1800; // 1800 - 2000 RESERVED for MENU COMMAND IDs

		for (auto& menu : mMenuBar->GetVectMenus()) {
			HMENU hMenuPopup = BuildMenu(menu, iMenuID);
			DWORD flags = MF_POPUP;
			if (!menu->IsEnabled()) flags |= MF_GRAYED;
			AppendMenu(hMenu, flags, (UINT_PTR)hMenuPopup, StringToWString(menu->GetLabel()).c_str());
		}

		SetMenu(hwnd, hMenu);
		DrawMenuBar(hwnd);
		return 1;
	}

	case WM_COMMAND:
	{
		int menuID = LOWORD(wParam);
		int index = menuID - 1800;
		if (index >= 0 && index < (int)AllVectMenuItems.size())
		{
			MenuItem* clickedItem = AllVectMenuItems[index];
			if (clickedItem && clickedItem->IsEnabled())
			{
				auto callback = clickedItem->GetOnClick();
				if (callback) callback(clickedItem);
			}
		}
		return 1;
	}
	case WM_SYSCOMMAND:
	{
		switch (wParam & 0xFFF0)
		{
		case SC_MINIMIZE:
		{
			OnMinimizeRequested();
			return 0;
		}
		case SC_RESTORE:
		{
			OnRestoreRequested();
			return 0;
		}
		}
		break;
	}
	case WM_CLOSE: {
		bool allowClose = true;
		if (mBeforeClose)
		{
			allowClose = mBeforeClose(this->GetWindowTitle(this->getWndContext().hWnd));
		}
		if (allowClose)
		{
			DestroyWindow(hwnd); 
		}
		return 0;
	}
	case WM_SETFOCUS:
		this->SetFocus(true);
		break;
	case WM_KILLFOCUS:
		this->SetFocus(false);
		break;
	case WM_CHAR:
	{
		if (mOnKeyDown)
		{
			KeyEvent e{};
			e.keyType = Key::Character;
			e.ch = (char)wParam;

			mLastChar = e.ch;

			e.ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			e.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			e.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

			e.state = KeyEventState::NotHandled;
			mOnKeyDown(e);
		}
		break;
	}

	case WM_KEYDOWN:
	{
		if (mOnKeyDown)
		{
			KeyEvent e{};
			e.isRepeat = (lParam & (1 << 30)) != 0;

			e.ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			e.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			e.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case VK_ESCAPE: e.keyType = Key::Escape; break;
			case VK_RETURN: e.keyType = Key::Enter; break;
			case VK_BACK:   e.keyType = Key::Backspace; break;
			case VK_TAB:    e.keyType = Key::Tab; break;
			case VK_LEFT:   e.keyType = Key::Left; break;
			case VK_RIGHT:  e.keyType = Key::Right; break;
			case VK_UP:     e.keyType = Key::Up; break;
			case VK_DOWN:   e.keyType = Key::Down; break;
			default:
				return 0;
			}
			e.state = KeyEventState::NotHandled;
			mOnKeyDown(e);
		}
		break;
	}
	case WM_KEYUP:
	{
		if (mOnKeyUp)
		{
			KeyEvent e{};
			e.isRepeat = false;

			e.ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			e.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			e.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

			BYTE keyboardState[256];
			GetKeyboardState(keyboardState);

			WCHAR buffer[2];
			int result = ToUnicode(
				(UINT)wParam,
				(lParam >> 16) & 0xFF,
				keyboardState,
				buffer,
				2,
				0
			);

			if (result == 1)
			{
				e.keyType = Key::Character;
				e.ch = (char)buffer[0];
			}
			else
			{
				switch (wParam)
				{
				case VK_ESCAPE: e.keyType = Key::Escape; break;
				case VK_RETURN: e.keyType = Key::Enter; break;
				case VK_BACK:   e.keyType = Key::Backspace; break;
				case VK_TAB:    e.keyType = Key::Tab; break;
				case VK_LEFT:   e.keyType = Key::Left; break;
				case VK_RIGHT:  e.keyType = Key::Right; break;
				case VK_UP:     e.keyType = Key::Up; break;
				case VK_DOWN:   e.keyType = Key::Down; break;
				default:
					return 0;
				}
			}

			e.state = KeyEventState::NotHandled;
			mOnKeyUp(e);
		}
		break;
	}

	case WM_DESTROY:
		this->mOnClose(WStringToString(mWindowName));
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void NAMESPACE_FLUENTX::MainWindow::SetMenuBar(MenuBar* mBar)
{
	this->mMenuBar = mBar;
	this->mMenuBarSet = true;
}

void NAMESPACE_FLUENTX::MainWindow::UseMenuBar(bool use)
{
	this->mUseMenuBar = use;
}

bool NAMESPACE_FLUENTX::MainWindow::IsUsingMenuBar()
{
	return this->mUseMenuBar;
}

NAMESPACE_FLUENTX::MenuBar* NAMESPACE_FLUENTX::MainWindow::getMenuBar()
{
	if (!mMenuBarSet) {
		std::string err = "GetMenuBar failed!\nMenu Bar Was Never Set For Window : " + WStringToString(mWindowName);
		FLUENTX_THROW_ERROR(err);
	}
	return mMenuBar;
}

void NAMESPACE_FLUENTX::MainWindow::OnKeyDown(OnWndKeyDown func)
{
	this->mOnKeyDown = func;
}

void NAMESPACE_FLUENTX::MainWindow::OnKeyUp(OnWndKeyUp func)
{
	this->mOnKeyUp = func;
}

void NAMESPACE_FLUENTX::MainWindow::setMainWndTransSet(MainWindowTransitionSet set)
{
	this->mMainWndTransSet = set;
}

NAMESPACE_FLUENTX::MainWindowTransitionSet& NAMESPACE_FLUENTX::MainWindow::getMainWndTransSet()
{
	return this->mMainWndTransSet;
}

HMENU NAMESPACE_FLUENTX::MainWindow::BuildMenu(Menu* menu, int& iMenuID)
{
	HMENU hMenuPopup = CreateMenu();

	for (auto& item : menu->GetVectMenuItems()) {
		if (item->GetLabel() == FLUENTX_STR_MENU_SEP) {
			AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
		}
		else if (item->_GetIsSubMenu()) {
			//std::cout << "Got Sub Menu\n";
			HMENU hSubMenu = BuildMenu(item->_GetSubMenu(), iMenuID);
			AppendMenu(hMenuPopup, MF_POPUP, (UINT_PTR)hSubMenu, StringToWString(item->GetLabel()).c_str());
		}
		else {
			DWORD flags = MF_STRING;
			if (item->IsChecked()) flags |= MF_CHECKED;
			if (!item->IsEnabled()) flags |= MF_GRAYED;
			AppendMenu(hMenuPopup, flags, iMenuID, StringToWString(item->GetLabel()).c_str());
			AllVectMenuItems.push_back(item);
			iMenuID++;
		}
	}

	return hMenuPopup;
}

void NAMESPACE_FLUENTX::MainWindow::OnMinimizeRequested()
{
	HWND hwnd = this->getWndContext().hWnd;
	MainWindowMinimizeTransition& MinTrans = this->getMainWndTransSet().minimize;
	WindowAnimEffect effect = MinTrans.animation.effect;

	if (!MinTrans.enabled || effect == WindowAnimEffect::None) {
		::ShowWindow(hwnd, SW_MINIMIZE);
		return;
	}

	RECT rc;
	GetWindowRect(hwnd, &rc);
	this->mRestoreRect = rc;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HMONITOR hMon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hMon, &mi);
	const RECT& wa = mi.rcWork;

	HDC hdcWin = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcWin);

	BITMAPINFOHEADER bi = {};
	bi.biSize = sizeof(bi);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	void* pvBits = nullptr;
	HBITMAP bmp = CreateDIBSection(hdcWin, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pvBits, nullptr, 0);
	SelectObject(hdcMem, bmp);
	PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);

	auto* pixels = static_cast<DWORD*>(pvBits);
	for (int i = 0; i < width * height; ++i)
		pixels[i] |= 0xFF000000;

	HWND hAnimWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		L"STATIC", nullptr, WS_POPUP,
		rc.left, rc.top, width, height,
		nullptr, nullptr, GetModuleHandle(nullptr), nullptr
	);

	POINT src = { 0, 0 };
	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = 255;

	POINT initDst0 = { rc.left, rc.top };
	SIZE  initSz0 = { width, height };
	UpdateLayeredWindow(hAnimWnd, nullptr, &initDst0, &initSz0, hdcMem, &src, 0, &blend, ULW_ALPHA);

	::ShowWindow(hwnd, SW_HIDE);
	::ShowWindow(hAnimWnd, SW_SHOWNA);

	ANIMATIONINFO ai = { sizeof(ai) };
	SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);
	int savedAnim = ai.iMinAnimate;
	ai.iMinAnimate = 0;
	SystemParametersInfo(SPI_SETANIMATION, sizeof(ai), &ai, 0);

	const int duration = MinTrans.animation.durationMS;
	timeBeginPeriod(1);
	auto start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto  now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::milli>(now - start).count();
		float p = (elapsed / static_cast<float>(duration) < 1.0f) ? elapsed / static_cast<float>(duration) : 1.0f;
		float e = p * p * p;
		float s = 1.0f - e;

		int newW = width, newH = height;
		int newX = rc.left, newY = rc.top;

		switch (effect)
		{
		case WindowAnimEffect::Fade:
			break;
		case WindowAnimEffect::Zoom:
			newW = max(1, static_cast<int>(width * s));
			newH = max(1, static_cast<int>(height * s));
			newX = rc.left + (width - newW) / 2;
			newY = rc.top + (height - newH) / 2;
			break;
		case WindowAnimEffect::SlideTop:
			newH = max(1, static_cast<int>(height * s));
			newY = static_cast<int>(rc.top * (1.0f - e) + wa.top * e);
			break;
		case WindowAnimEffect::SlideBottom:
			newH = max(1, static_cast<int>(height * s));
			newY = static_cast<int>(rc.top * (1.0f - e) + (wa.bottom - newH) * e);
			break;
		case WindowAnimEffect::SlideLeft:
			newW = max(1, static_cast<int>(width * s));
			newX = static_cast<int>(rc.left * (1.0f - e) + (wa.left - newW) * e);
			break;
		case WindowAnimEffect::SlideRight:
			newW = max(1, static_cast<int>(width * s));
			newX = static_cast<int>(rc.left * (1.0f - e) + wa.right * e);
			break;
		default:
			break;
		}

		BLENDFUNCTION b = blend;
		b.SourceConstantAlpha = MinTrans.animation.fade ? static_cast<BYTE>(255 * (1.0f - p)) : 255;

		POINT dst0 = { newX, newY };
		SIZE  sz0 = { newW, newH };
		UpdateLayeredWindow(hAnimWnd, nullptr, &dst0, &sz0, hdcMem, &src, 0, &b, ULW_ALPHA);

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (p >= 1.0f) break;
		DwmFlush();
	}

	timeEndPeriod(1);

	DestroyWindow(hAnimWnd);
	DeleteObject(bmp);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcWin);

	::ShowWindow(hwnd, SW_MINIMIZE);
	ai.iMinAnimate = savedAnim;
	SystemParametersInfo(SPI_SETANIMATION, sizeof(ai), &ai, 0);
}

void NAMESPACE_FLUENTX::MainWindow::OnRestoreRequested()
{
	HWND hwnd = this->getWndContext().hWnd;
	MainWindowRestoreTransition& ResTrans = this->getMainWndTransSet().restore;
	WindowAnimEffect effect = ResTrans.animation.effect;

	if (!ResTrans.enabled || effect == WindowAnimEffect::None) {
		::ShowWindow(hwnd, SW_RESTORE);
		return;
	}

	RECT rc = this->mRestoreRect;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HMONITOR hMon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hMon, &mi);
	const RECT& wa = mi.rcWork;

	::ShowWindow(hwnd, SW_RESTORE);
	::ShowWindow(hwnd, SW_HIDE);

	HDC hdcWin = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcWin);

	BITMAPINFOHEADER bi = {};
	bi.biSize = sizeof(bi);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	void* pvBits = nullptr;
	HBITMAP bmp = CreateDIBSection(hdcWin, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pvBits, nullptr, 0);
	SelectObject(hdcMem, bmp);
	PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);

	auto* pixels = static_cast<DWORD*>(pvBits);
	for (int i = 0; i < width * height; ++i)
		pixels[i] |= 0xFF000000;

	HWND hAnimWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		L"STATIC", nullptr, WS_POPUP,
		rc.left, rc.top, width, height,
		nullptr, nullptr, GetModuleHandle(nullptr), nullptr
	);

	POINT src = { 0, 0 };
	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = 255;

	POINT initDst1 = { rc.left, rc.top };
	SIZE  initSz1 = { width, height };
	UpdateLayeredWindow(hAnimWnd, nullptr, &initDst1, &initSz1, hdcMem, &src, 0, &blend, ULW_ALPHA);
	::ShowWindow(hAnimWnd, SW_SHOWNA);

	const int duration = ResTrans.animation.durationMS;
	timeBeginPeriod(1);
	auto start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto  now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::milli>(now - start).count();
		float p = (elapsed / static_cast<float>(duration) < 1.0f) ? elapsed / static_cast<float>(duration) : 1.0f;
		float e = 1.0f - pow(1.0f - p, 3.0f);
		float s = e;

		int newW = width, newH = height;
		int newX = rc.left, newY = rc.top;

		switch (effect)
		{
		case WindowAnimEffect::Fade:
			break;
		case WindowAnimEffect::Zoom:
			newW = max(1, static_cast<int>(width * s));
			newH = max(1, static_cast<int>(height * s));
			newX = rc.left + (width - newW) / 2;
			newY = rc.top + (height - newH) / 2;
			break;
		case WindowAnimEffect::SlideTop:
			newH = max(1, static_cast<int>(height * s));
			newY = static_cast<int>(wa.top * (1.0f - e) + rc.top * e);
			break;
		case WindowAnimEffect::SlideBottom:
			newH = max(1, static_cast<int>(height * s));
			newY = static_cast<int>((wa.bottom - newH) * (1.0f - e) + rc.top * e);
			break;
		case WindowAnimEffect::SlideLeft:
			newW = max(1, static_cast<int>(width * s));
			newX = static_cast<int>((wa.left - newW) * (1.0f - e) + rc.left * e);
			break;
		case WindowAnimEffect::SlideRight:
			newW = max(1, static_cast<int>(width * s));
			newX = static_cast<int>(wa.right * (1.0f - e) + rc.left * e);
			break;
		default:
			break;
		}

		BLENDFUNCTION b = blend;
		b.SourceConstantAlpha = ResTrans.animation.fade ? static_cast<BYTE>(255 * p) : 255;

		POINT dst1 = { newX, newY };
		SIZE  sz1 = { newW, newH };
		UpdateLayeredWindow(hAnimWnd, nullptr, &dst1, &sz1, hdcMem, &src, 0, &b, ULW_ALPHA);

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (p >= 1.0f) break;
		DwmFlush();
	}

	timeEndPeriod(1);

	::ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	DestroyWindow(hAnimWnd);
	DeleteObject(bmp);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcWin);
}