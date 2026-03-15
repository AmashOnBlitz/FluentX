#pragma once
#include "window.h"
#include "core/config.h"
#include <string>

namespace NAMESPACE_FLUENTX {

	class MainWindow : public Window {
	public:
		MainWindow();
		~MainWindow();

		bool Init(std::string windowName, int height, int weight, int xPos, int yPos);

		LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private: //Vars
		std::wstring mWindowName = L"";
		std::wstring mClassName = L"";
	};

}