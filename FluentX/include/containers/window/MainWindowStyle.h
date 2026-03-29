#pragma once
#include "core/config.h"
#include <cstdint>

namespace NAMESPACE_FLUENTX {

    enum class MainWindowCreationFlags : uint32_t
    {
        None = 0,

        TitleBar = 1 << 0,
        Border = 1 << 1,
        Resizable = 1 << 2,
        MinimizeButton = 1 << 3,
        MaximizeButton = 1 << 4,
        SystemMenu = 1 << 5,
        Popup = 1 << 6,
        Dialog = 1 << 7,

        TopMost = 1 << 8,
        ToolWindow = 1 << 9,
        AppWindow = 1 << 10,
        AcceptFiles = 1 << 11
    };

    inline constexpr MainWindowCreationFlags operator|(MainWindowCreationFlags a, MainWindowCreationFlags b)
    {
        return static_cast<MainWindowCreationFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
            );
    }

    inline constexpr MainWindowCreationFlags operator&(MainWindowCreationFlags a, MainWindowCreationFlags b)
    {
        return static_cast<MainWindowCreationFlags>(
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
            );
    }


    enum class MainWindowUIFlags : uint32_t
    {
        None = 0,

        DropShadow = 1 << 0,
        Layered = 1 << 1,
        BlurBehind = 1 << 2,
        Acrylic = 1 << 3,
        RoundedCorners = 1 << 4
    };

    inline constexpr MainWindowUIFlags operator|(MainWindowUIFlags a, MainWindowUIFlags b)
    {
        return static_cast<MainWindowUIFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
            );
    }

    inline constexpr MainWindowUIFlags operator&(MainWindowUIFlags a, MainWindowUIFlags b)
    {
        return static_cast<MainWindowUIFlags>(
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
            );
    }


    enum class MainWindowBehaviorFlags : uint32_t
    {
        None = 0,

        ClickThrough = 1 << 0,
        Transparent = 1 << 1,
        NoActivate = 1 << 2,
        DisableAltF4 = 1 << 3
    };

    inline constexpr MainWindowBehaviorFlags operator|(MainWindowBehaviorFlags a, MainWindowBehaviorFlags b)
    {
        return static_cast<MainWindowBehaviorFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
            );
    }

    inline constexpr MainWindowBehaviorFlags operator&(MainWindowBehaviorFlags a, MainWindowBehaviorFlags b)
    {
        return static_cast<MainWindowBehaviorFlags>(
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
            );
    }


    enum class MainWindowStartupState
    {
        Normal,
        Minimized,
        Maximized,
        Hidden,
        Restore,
        Show,
        ShowNoActivate,
        ShowDefault
    };


    struct MainWindowStyle
    {
        MainWindowCreationFlags creation = MainWindowCreationFlags::None;
        MainWindowUIFlags ui = MainWindowUIFlags::None;
        MainWindowBehaviorFlags behavior = MainWindowBehaviorFlags::None;
        MainWindowStartupState startup = MainWindowStartupState::Normal;
    };


    namespace MainWindowStylePresets {

        inline constexpr MainWindowStyle DefaultWindow{
            MainWindowCreationFlags::TitleBar |
            MainWindowCreationFlags::Border |
            MainWindowCreationFlags::Resizable |
            MainWindowCreationFlags::MinimizeButton |
            MainWindowCreationFlags::MaximizeButton |
            MainWindowCreationFlags::SystemMenu |
            MainWindowCreationFlags::AppWindow,

            MainWindowUIFlags::DropShadow,

            MainWindowBehaviorFlags::None,

            MainWindowStartupState::Normal
        };


        inline constexpr MainWindowStyle Borderless{
            MainWindowCreationFlags::AppWindow,

            MainWindowUIFlags::None,

            MainWindowBehaviorFlags::None,

            MainWindowStartupState::Normal
        };


        inline constexpr MainWindowStyle Dialog{
            MainWindowCreationFlags::TitleBar |
            MainWindowCreationFlags::Border |
            MainWindowCreationFlags::SystemMenu |
            MainWindowCreationFlags::Dialog,

            MainWindowUIFlags::DropShadow,

            MainWindowBehaviorFlags::None,

            MainWindowStartupState::Normal
        };


        inline constexpr MainWindowStyle ToolPanel{
            MainWindowCreationFlags::TitleBar |
            MainWindowCreationFlags::Border |
            MainWindowCreationFlags::ToolWindow |
            MainWindowCreationFlags::TopMost,

            MainWindowUIFlags::DropShadow,

            MainWindowBehaviorFlags::NoActivate,

            MainWindowStartupState::ShowNoActivate
        };


        inline constexpr MainWindowStyle Popup{
            MainWindowCreationFlags::Popup,

            MainWindowUIFlags::DropShadow,

            MainWindowBehaviorFlags::None,

            MainWindowStartupState::Show
        };


        inline constexpr MainWindowStyle Overlay{
            MainWindowCreationFlags::TopMost,

            MainWindowUIFlags::Layered,

            MainWindowBehaviorFlags::ClickThrough |
            MainWindowBehaviorFlags::Transparent |
            MainWindowBehaviorFlags::NoActivate,

            MainWindowStartupState::Show
        };

    }

    enum class WindowTransitionDirection
    {
        None,
        Top,
        Bottom,
        Left,
        Right
    };

    struct MainWindowTransition {
    public:
        WindowTransitionDirection direction = WindowTransitionDirection::Bottom;
        int durationMS = 200;
    };

    struct MainWindowStyleTransition { // base ---
    public:
        bool enabled = false;
        MainWindowTransition animation;
    };

    struct MainWindowMinimizeTransition : public MainWindowStyleTransition
    {
    public:
        bool slide = true;
        bool scale = true;
        bool fade = true;
    };

    struct MainWindowRestoreTransition : public MainWindowStyleTransition
    {
    public:
        bool slide = true;
        bool scale = true;
        bool fade = true;
    };

    //struct WindowMaximizeTransition : public WindowStyleTransition
    //{
    //};

    struct MainWindowTransitionSet {
    public:
        MainWindowMinimizeTransition minimize{};
        MainWindowRestoreTransition restore{};
    };

}
