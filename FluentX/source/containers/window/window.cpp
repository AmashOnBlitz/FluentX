#include "containers/window/window.h"
#include "core/macroFuncs.h"
#include <containers/window/MainWindow.h>

NAMESPACE_FLUENTX::Window::Window()
{
	mWndContext = new WindowContext{};
}


NAMESPACE_FLUENTX::Window::~Window()
{
	if (mWndContext) delete mWndContext;
	mWndContext = nullptr;
}

WindowContext& NAMESPACE_FLUENTX::Window::getWndContext()
{
	if (!mWndContext) {
		FLUENTX_THROW_ERROR("WindowContext not set!");
	}
	return (*this->mWndContext);
}

bool NAMESPACE_FLUENTX::Window::setWndContext(WindowContext& wndCont)
{
	this->mWndContext = &wndCont;
	return true;
}

bool NAMESPACE_FLUENTX::Window::showWindow()
{
	if (mWndContext) {
		if (mWndContext->hWnd != INVALID_HANDLE_VALUE && mWndContext->hWnd != NULL) {
			::ShowWindow(mWndContext->hWnd, SW_NORMAL);
			::UpdateWindow(mWndContext->hWnd);
			return true;
		}
	}
	std::string err = "HWND not found, maybe incomplete window creation!";
	this->setLastErr(err);
	FLUENTX_THROW_ERROR(err);
	return false;
}

bool NAMESPACE_FLUENTX::Window::hideWindow()
{
	if (mWndContext) {
		if (mWndContext->hWnd != INVALID_HANDLE_VALUE && mWndContext->hWnd != NULL) {
			::ShowWindow(mWndContext->hWnd, SW_HIDE);
			return true;
		}
	}
	std::string err = "HWND not found, maybe incomplete window creation!";
	this->setLastErr(err);
	FLUENTX_THROW_ERROR(err);
	return false;
}

void NAMESPACE_FLUENTX::Window::onClose(OnWindowClose func)
{
	mOnClose = func;
}

std::string NAMESPACE_FLUENTX::Window::fetchLastErr()
{
	std::string err = this->errStr;
	this->errStr = "";
	return err;
}

void NAMESPACE_FLUENTX::Window::setLastErr(std::string _e)
{
	this->errStr = _e;
}

LRESULT NAMESPACE_FLUENTX::fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MainWindow* MainWnd = nullptr;
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		MainWnd = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)MainWnd);
	}
	else {
		MainWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (MainWnd) {
		return MainWnd->fnWndProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
