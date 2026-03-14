#include "containers/window/MainWindow.h"
#include "core/WindowsConstants.h"
#include "core/macroFuncs.h"
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
	wndClassEx.hbrBackground = NULL;
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
			std::string(mWindowName.begin(), mWindowName.end()) +
			"\"!\nError Code: " + std::to_string(errorCode);
		FLUENTX_THROW_ERROR(err);
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
}

std::string& NAMESPACE_FLUENTX::MainWindow::fetchLastErr()
{
	return this->errStr;
}

void NAMESPACE_FLUENTX::MainWindow::onClose(OnMainWindowClose func)
{
	mOnClose = func;
}

LRESULT NAMESPACE_FLUENTX::MainWindow::fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY: 
	{
		this->mOnClose(std::string(mWindowName.begin(), mWindowName.end()));
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
