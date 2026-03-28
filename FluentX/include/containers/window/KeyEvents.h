#pragma once
#include "core/config.h"
#include <functional>

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
	private:
		char mLastChar = 0;
	};

	typedef std::function<void(KeyEvent)> OnWndKeyDown;
	typedef std::function<void(KeyEvent)> OnWndKeyUp;
}