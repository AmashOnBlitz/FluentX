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

bool NAMESPACE_FLUENTX::MainWindow::Init(std::string windowName, int height, int weight, int xPos, int yPos)
{
	static int windowGenCount = 0;
	mWindowName = std::wstring(windowName.begin(), windowName.end()); 
	mClassName = L"Class_MAINWINDOW_" + mWindowName + std::to_wstring(++windowGenCount);
	WNDCLASSEX wndClassEx{};
	wndClassEx.cbSize = sizeof(wndClassEx);
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	//wndClassEx.hbrBackground = NULL;
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
	HWND hWnd = CreateWindowEx(
		0,
		mClassName.c_str(),
		mWindowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		xPos,
		yPos,
		weight,
		height,
		NULL,
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
	this->getWndContext().hWnd = hWnd;
	return true;
}

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
