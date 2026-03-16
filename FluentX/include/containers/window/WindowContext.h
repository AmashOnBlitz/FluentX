#pragma once
#include "Windows.h"

struct WindowContext {
	HWND hWnd{};
	WNDCLASSEX wndClassEx{};
	HWND hParent{ NULL };
};