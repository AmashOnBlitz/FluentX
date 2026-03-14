#pragma once
#include "window.h"
#include "core/config.h"
#include <string>

namespace NAMESPACE_FLUENTX {

	typedef void (*OnMainWindowClose)(std::string windowName);
	class MainWindow : public Window {
	public:
		MainWindow();
		~MainWindow();

		bool Init(std::string windowName, int height, int weight, int xPos, int yPos);
		std::string& fetchLastErr();

		void onClose(OnMainWindowClose func);

		LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		std::wstring mWindowName = L"";
		std::wstring mClassName = L"";
		std::string errStr = "";
		OnMainWindowClose mOnClose = nullptr;
	};

}