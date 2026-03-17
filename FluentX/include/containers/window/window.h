#pragma once
#include "WindowContext.h"
#include <string>
#include <functional>
#include "core/config.h"
#include "MainWindowStyle.h"
#include <dwmapi.h>

namespace NAMESPACE_FLUENTX {

	typedef std::function<void(std::string)> OnWindowClose;
	typedef std::function<bool(std::string)> BeforeWindowClose;

	class Window {
	public:
		Window();
		~Window();

		WindowContext& getWndContext();
		bool setWndContext(WindowContext& wndCont);

		bool ShowWindow();
		bool HideWindow();

		void onClose(OnWindowClose func);
		void BeforeClose(BeforeWindowClose func);
		std::string FetchLastErr();

		void SetPos(int x, int y);
		void SetDimensions(int width, int height);
		void SetBounds(int x, int y, int width, int height);

		int GetPosX();
		int GetPosY();
		int GetWidth();
		int GetHeight();

		bool saveWindowData();
		void loadWindowData();

	protected:
		void setLastErr(std::string _e);
		DWORD ConvToWin32WndStyle_Creation(MainWindowCreationFlags style);
		DWORD ConvToWin32WndExStyle_Creation(MainWindowCreationFlags style);
		int ConvToWin32WndStyle_Startup(MainWindowStartupState state);
		void ApplyUIFlags(HWND hwnd, MainWindowUIFlags flags);
		void ApplyBehaviorFlags(HWND hwnd, MainWindowBehaviorFlags flags);
		std::string GetWindowTitle(HWND hwnd);

	private: //VARS
		WindowContext* mWndContext = nullptr;
		std::string errStr = "";
		RECT mRect{};

	protected:
		OnWindowClose mOnClose = nullptr;
		BeforeWindowClose mBeforeClose = nullptr;
		MainWindowStyle mStyle{};
	};

	LRESULT CALLBACK fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}