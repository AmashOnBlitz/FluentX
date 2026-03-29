#pragma once
#include "window.h"
#include "core/config.h"
#include "MainWindowStyle.h"
#include <string>
#include "containers/Menu/MenuBar.h"
#include "core/KeyEvents.h"
#include <core/MouseEvents.h>

namespace NAMESPACE_FLUENTX {

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
            MainWindow* parent = nullptr,
            MainWindowStyle style = MainWindowStylePresets::DefaultWindow,
            MainWindowTransitionSet transitions = MainWindowTransitionSet{}
        );

        LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void SetMenuBar(MenuBar* mBar);
        void UseMenuBar(bool use);
        bool IsUsingMenuBar();
        MenuBar* getMenuBar();

        void OnKeyDown(OnWndKeyDown func);
        void OnKeyUp(OnWndKeyUp func);

        void OnMouseEvent(std::function<void(MouseEvent&)> fn);

        void setMainWndTransSet(MainWindowTransitionSet set);
        MainWindowTransitionSet& getMainWndTransSet();

    private: //Funcs
        HMENU BuildMenu(Menu* menu, int& iMenuID);
        void OnMinimizeRequested();
        void OnRestoreRequested();
        void DispatchMouseEvent(
            HWND hwnd,
            MouseEventType type,
            MouseButton btn,
            WPARAM wParam,
            LPARAM lParam,
            int delta = 0,
            bool screenCoords = false
        );

    private: //Vars
        bool mMenuBarSet = false;
        bool mUseMenuBar = true;
        bool mIsAnimatingMinimize = false;
        MenuBar* mMenuBar = nullptr;
        std::wstring mWindowName = L"";
        std::wstring mClassName = L"";
        std::vector<MenuItem*> AllVectMenuItems;
        OnWndKeyDown mOnKeyDown;
        OnWndKeyUp mOnKeyUp;
        mOnWndMouseEvent mOnMouseEvent;
        char mLastChar = 0;
        MainWindowTransitionSet mMainWndTransSet{};
        RECT mRestoreRect{};
    };

}