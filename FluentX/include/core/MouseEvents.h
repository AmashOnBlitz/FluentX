#pragma once
#include <core/config.h>
#include <functional>

namespace NAMESPACE_FLUENTX {

    enum class MouseEventType {
        Move,
        Down,
        Up,
        DoubleClick,
        Scroll,
        HScroll
    };

    enum class MouseButton {
        None,
        Left,
        Right,
        Middle,
        X1,
        X2
    };

    struct MouseEvent {
        int x = 0;
        int y = 0;

        MouseEventType type = MouseEventType::Move;
        MouseButton button = MouseButton::None;

        int delta = 0;
        bool handled = false;
    };
    typedef std::function<void(MouseEvent&)> mOnWndMouseEvent;
}
