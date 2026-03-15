#include "core/App.h"
#include "core/macroFuncs.h"
#include <cstdlib>
#include "Windows.h"
#include <iostream>

NAMESPACE_FLUENTX::App& NAMESPACE_FLUENTX::App::Instance()
{
	static App instance;
	return instance;
}

void NAMESPACE_FLUENTX::App::SetMainWindow(MainWindow* wnd)
{
	this->mMainWindow = wnd;
}

void NAMESPACE_FLUENTX::App::RegisterWindow(MainWindow* wnd)
{
	mRegisteredWindows.push_back(wnd);
}

void NAMESPACE_FLUENTX::App::UnregisterWindow(MainWindow* wnd)
{
	mRegisteredWindows.erase(
		std::remove_if(
			mRegisteredWindows.begin(),
			mRegisteredWindows.end(),
			[wnd](MainWindow* w) {
				return w == wnd; 
			}
		),
		mRegisteredWindows.end()
	);
}


int NAMESPACE_FLUENTX::App::Run()
{
	if (!mMainWindow) {
		FLUENTX_THROW_ERROR("Cannot Run App!\nIssue: Main Window Not Set");
		return EXIT_FAILURE;
	}
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return EXIT_SUCCESS;
}

void NAMESPACE_FLUENTX::App::Shutdown()
{
	for (auto win : mRegisteredWindows) {
		if (win) {
			win->hideWindow();
			std::cout << "Win Hidden (Class Name) : " << WStringToString(win->getWndContext().wndClassEx.lpszClassName) << "\n";
			::SendMessage(win->getWndContext().hWnd, WM_CLOSE, NULL, NULL);
			std::cout << "Win Closed (Class Name) : " << WStringToString(win->getWndContext().wndClassEx.lpszClassName) << "\n";
		}
	}
	if (mMainWindow) mMainWindow->hideWindow();
	PostQuitMessage(0);
}