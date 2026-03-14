#pragma once
#include "WindowContext.h"
#include "core/config.h"

namespace NAMESPACE_FLUENTX {

	class Window {
	public:
		Window();
		~Window();

		WindowContext& getWndContext();
		bool setWndContext(WindowContext& wndCont);

	private:
		WindowContext* wndContext = nullptr;
	};

	inline LRESULT CALLBACK fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}