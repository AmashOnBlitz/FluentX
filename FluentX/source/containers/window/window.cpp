#include "containers/window/window.h"
#include "core/macroFuncs.h"
#include <containers/window/MainWindow.h>

NAMESPACE_FLUENTX::Window::Window()
{
	wndContext = new WindowContext{};
}


NAMESPACE_FLUENTX::Window::~Window()
{
	if (wndContext) delete wndContext;
	wndContext = nullptr;
}

WindowContext& NAMESPACE_FLUENTX::Window::getWndContext()
{
	if (!wndContext) {
		FLUENTX_THROW_ERROR("WindowContext not set!");
	}
	return (*this->wndContext);
}

bool NAMESPACE_FLUENTX::Window::setWndContext(WindowContext& wndCont)
{
	this->wndContext = &wndCont;
	return true;
}

LRESULT NAMESPACE_FLUENTX::fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MainWindow* MainWnd = nullptr;
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		MainWnd = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)MainWnd);
	}
	else {
		MainWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (MainWnd) {
		return MainWnd->fnWndProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
