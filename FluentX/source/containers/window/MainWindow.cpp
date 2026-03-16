#include "containers/window/MainWindow.h"
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
	MainWindowStyle style,
	MainWindow* parent
)
{
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
	switch (msg)
	{
	case WM_DESTROY: 
		this->mOnClose(WStringToString(mWindowName));
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
