#pragma once
#include "window.h"
#include "core/config.h"
#include "MainWindowStyle.h"
#include <string>
#include "containers/Menu/MenuBar.h"

namespace NAMESPACE_FLUENTX {

	enum class Key {
		Unknown = 0,
		Escape,
		Enter,
		Backspace,
		Tab,
		Left,
		Right,
		Up,
		Down,
		Character
	};
	enum class KeyEventState {
		Handled = 0,
		NotHandled
	};

	struct KeyEvent {
		Key keyType;
		KeyEventState state;
		char ch;
		bool isRepeat;
		bool ctrl;
		bool shift;
		bool alt;
	};

	typedef std::function<void(KeyEvent)> OnWndKeyDown;

	class MainWindow : public Window {
	public:
		MainWindow();
		~MainWindow();

		bool Init(
			std::string windowName,
			int width,
			int height,
			int xPos,
			int yPos,
			MainWindowStyle style = MainWindowStylePresets::DefaultWindow,
			MainWindow* parent = nullptr
		);

		LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void SetMenuBar(MenuBar* mBar);
		void UseMenuBar(bool use);
		bool IsUsingMenuBar();
		MenuBar* getMenuBar();
		void OnKeyDown(OnWndKeyDown func);
	private: //Funcs
		HMENU BuildMenu(Menu* menu, int& iMenuID);
	private: //Vars
		bool mMenuBarSet = false;
		bool mUseMenuBar = true;
		MenuBar* mMenuBar = nullptr;
		std::wstring mWindowName = L"";
		std::wstring mClassName = L""; 
		std::vector<MenuItem*> AllVectMenuItems;
		OnWndKeyDown mOnKeyDown;
	};

}