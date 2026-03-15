#pragma once
#include "WindowContext.h"
#include <string>
#include "core/config.h"

namespace NAMESPACE_FLUENTX {

	typedef void (*OnWindowClose)(std::string windowName);

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

	protected:
		void setLastErr(std::string _e);

	private: //VARS
		WindowContext* mWndContext = nullptr;
		std::string errStr = "";

	protected:
		OnWindowClose mOnClose = nullptr;
	};

	LRESULT CALLBACK fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}