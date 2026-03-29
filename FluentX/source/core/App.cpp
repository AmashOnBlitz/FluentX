#include "core/pch.h"
#include "core/App.h"
#include "core/macroFuncs.h"
#include <cstdlib>
#include "Windows.h"
#include <iostream>
#include <core/CustomWindowMessages.h>

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


void NAMESPACE_FLUENTX::App::Init()
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	this->isInit = true;
}

int NAMESPACE_FLUENTX::App::Run()
{
	if (!isInit) {
		FLUENTX_THROW_ERROR("(App::Run failed) Cannot Run App\nIssue: App not successfully init!");
		return EXIT_FAILURE;
	}
	if (!mMainWindow) {
		FLUENTX_THROW_ERROR("(App::Run failed) Cannot Run App!\nIssue: Main Window Not Set");
		return EXIT_FAILURE;
	}
	mRunning = true;

	OnUpdateFunc updFunc;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

	std::atomic<int> mRealFPS = 0;

	mUpdateThread = std::thread([this,&mRealFPS]() {

		OnUpdateFunc updFunc;
		int frameCounter = 0;
		auto lastFPSUpdate = std::chrono::steady_clock::now();

		while (mRunning)
		{
			auto frameStart = std::chrono::steady_clock::now();

			{
				std::lock_guard<std::mutex> lock(mUpdateMutex);
				updFunc = mOnUpdate;
			}
			CheckMenuRebuilds();
			if (updFunc)
				updFunc();

			frameCounter++;

			auto now = std::chrono::steady_clock::now();

			if (std::chrono::duration_cast<std::chrono::seconds>(now - lastFPSUpdate).count() >= 1)
			{
				mRealFPS = frameCounter;
				frameCounter = 0;
				lastFPSUpdate = now;
			}

			auto frameEnd = std::chrono::steady_clock::now();
			auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
			int targetFrameTime = 1000 / mFps.load();

			if (frameTime.count() < targetFrameTime)
			{
				std::this_thread::sleep_for(
					std::chrono::milliseconds(targetFrameTime - frameTime.count())
				);
			}
		}
	});


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return EXIT_SUCCESS;
}

void NAMESPACE_FLUENTX::App::Shutdown()
{
	mRunning = false;
	if (mUpdateThread.joinable()) mUpdateThread.join();

	for (auto& win : mRegisteredWindows) {
		if (win) {
			win->HideWindow();
			//std::cout << "Win Hidden (Class Name) : " << WStringToString(win->getWndContext().wndClassEx.lpszClassName) << "\n";
			::SendMessage(win->getWndContext().hWnd, WM_CLOSE, NULL, NULL);
			//std::cout << "Win Closed (Class Name) : " << WStringToString(win->getWndContext().wndClassEx.lpszClassName) << "\n";
		}
	}
	if (mMainWindow) mMainWindow->HideWindow();
	PostQuitMessage(0);
}

NAMESPACE_FLUENTX::OnUpdateFunc NAMESPACE_FLUENTX::App::getOnUpdate()
{
	return mOnUpdate;
}

void NAMESPACE_FLUENTX::App::OnUpdate(OnUpdateFunc func)
{
	mOnUpdate = func;
}

void NAMESPACE_FLUENTX::App::SetFPS(int fps)
{
	mFps = fps;
}

int NAMESPACE_FLUENTX::App::GetFPS()
{
	return mFps;
}

void NAMESPACE_FLUENTX::App::CheckMenuRebuilds()
{
	for (auto& win : mRegisteredWindows)
	{
		if (!win || !win->IsUsingMenuBar()) continue;

		MenuBar* bar = win->getMenuBar();
		if (bar->GetRebuild())
		{
			//std::cout << "Sending WM rebuild Menu\n";
			::SendMessage(win->getWndContext().hWnd, WM_FLUENTX_REBUILD_MENU, 0, 0);
			bar->FlagRebuild(false); 
		}
	}
	if (mMainWindow && mMainWindow->IsUsingMenuBar()) {
		MenuBar* bar = mMainWindow->getMenuBar();
		if (bar->GetRebuild())
		{
			//std::cout << "Sending WM rebuild Menu\n";
			::SendMessage(mMainWindow->getWndContext().hWnd, WM_FLUENTX_REBUILD_MENU, 0, 0);
			bar->FlagRebuild(false);
		}
	}
}

void NAMESPACE_FLUENTX::App::EnableDebugCout(bool enable)
{
	this->isDebugCoutEnabled = enable;
}

bool NAMESPACE_FLUENTX::App::GetDebugCoutEnabled()
{
	return this->isDebugCoutEnabled;
}

