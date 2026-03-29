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
		//std::cout << "(Re)Creating Menu\n";
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
			if (mIsAnimatingMinimize)
			{
				mIsAnimatingMinimize = false;
				break;
			}
			OnMinimizeRequested();
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
	WindowTransitionDirection& dir = MinTrans.animation.direction;
	if (!MinTrans.enabled) {
		::ShowWindow(hwnd, SW_MINIMIZE);
		return;
	}

	RECT rc;
	GetWindowRect(hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HDC hdcWin = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcWin);

	HBITMAP bmp = CreateCompatibleBitmap(hdcWin, width, height);
	SelectObject(hdcMem, bmp);

	PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);

	HWND hAnimWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		L"STATIC",
		nullptr,
		WS_POPUP,
		rc.left, rc.top,
		width, height,
		nullptr, nullptr,
		GetModuleHandle(nullptr),
		nullptr
	);

	POINT src = { 0, 0 };
	POINT dst = { rc.left, rc.top };
	SIZE size = { width, height };

	BLENDFUNCTION blend{};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	UpdateLayeredWindow(
		hAnimWnd,
		nullptr,
		&dst,
		&size,
		hdcMem,
		&src,
		0,
		&blend,
		ULW_ALPHA
	);

	::ShowWindow(hAnimWnd, SW_SHOW);
	::ShowWindow(hwnd, SW_HIDE);

	mIsAnimatingMinimize = true;
	::ShowWindow(hwnd, SW_MINIMIZE);

	int screenH = GetSystemMetrics(SM_CYSCREEN);

	int duration = MinTrans.animation.durationMS;
	auto start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::milli>(now - start).count();

		float p = elapsed / duration;
		if (p > 1.0f) p = 1.0f;

		float e = 1.0f - pow(1.0f - p, 3);

		int newX = rc.left;
		int newY = rc.top;
		int newW = width;
		int newH = height;
		newW = max(1, newW);
		newH = max(1, newH);

		if (MinTrans.slide)
		{
			switch (dir)
			{
			case WindowTransitionDirection::Bottom:
			{
				newH = (int)(height * (1.0f - e));
				newY = rc.top + (height - newH);
				break;
			}

			case WindowTransitionDirection::Top:
			{
				newH = (int)(height * (1.0f - e));
				newY = rc.top;
				break;
			}

			case WindowTransitionDirection::Right:
			{
				newW = (int)(width * (1.0f - e));
				newX = rc.left + (width - newW);
				break;
			}

			case WindowTransitionDirection::Left:
			{
				newW = (int)(width * (1.0f - e));
				newX = rc.left;
				break;
			}

			default:
				break;
			}
		}

		if (MinTrans.scale)
		{
			float s = (1.0f - e);

			int scaledW = (int)(width * s);
			int scaledH = (int)(height * s);
			int centerX = rc.left + width / 2;
			int centerY = rc.top + height / 2;

			newX = centerX - scaledW / 2;
			newY = centerY - scaledH / 2;
			newW = scaledW;
			newH = scaledH;
		}

		BYTE alpha = 255;
		if (MinTrans.fade)
			alpha = (BYTE)(255 * (1.0f - p));

		SIZE newSize = { newW, newH };
		POINT newDst = { newX, newY };

		BLENDFUNCTION b = blend;
		b.SourceConstantAlpha = alpha;

		HDC hdcScaled = CreateCompatibleDC(hdcWin);
		HBITMAP bmpScaled = CreateCompatibleBitmap(hdcWin, newW, newH);
		SelectObject(hdcScaled, bmpScaled);
		StretchBlt(
			hdcScaled,
			0,
			0,
			newW,
			newH,
			hdcMem,
			0,
			0,
			width,
			height,
			SRCCOPY
		);

		UpdateLayeredWindow(
			hAnimWnd,
			nullptr,
			&newDst,
			&newSize,
			hdcScaled,
			&src,
			0,
			&b,
			ULW_ALPHA
		);

		if (p >= 1.0f) {
			DeleteObject(bmpScaled);
			DeleteDC(hdcScaled);
			break;
		}

		Sleep(8);
	}

	DestroyWindow(hAnimWnd);
	DeleteObject(bmp);
	DeleteDC(hdcMem);

	ReleaseDC(hwnd, hdcWin);

}
