#pragma once
#include "WindowContext.h"
#include <string>
#include <functional>
#include "core/config.h"
#include "MainWindowStyle.h"
#include <dwmapi.h>

namespace NAMESPACE_FLUENTX {

	typedef std::function<void(std::string)> OnWindowClose;

	class Window {
	public:
		Window();
		~Window();

		WindowContext& getWndContext();
		bool setWndContext(WindowContext& wndCont);

		bool showWindow();
		bool hideWindow();

		void onClose(OnWindowClose func);
		std::string fetchLastErr();

		void SetPos(int x, int y);
		void SetDimensions(int width, int height);
		void SetBounds(int x, int y, int width, int height);

		int GetPosX();
		int GetPosY();
		int GetWidth();
		int GetHeight();

	protected:
		void setLastErr(std::string _e);
		DWORD ConvToWin32WndStyle_Creation(MainWindowCreationFlags style);
		DWORD ConvToWin32WndExStyle_Creation(MainWindowCreationFlags style);
		int ConvToWin32WndStyle_Startup(MainWindowStartupState state);
		void ApplyUIFlags(HWND hwnd, MainWindowUIFlags flags);
		void ApplyBehaviorFlags(HWND hwnd, MainWindowBehaviorFlags flags);

	private: //VARS
		WindowContext* mWndContext = nullptr;
		std::string errStr = "";
		RECT mRect{};

	protected:
		OnWindowClose mOnClose = nullptr;
		MainWindowStyle mStyle{};
	};

	LRESULT CALLBACK fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}