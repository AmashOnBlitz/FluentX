#pragma once
#include "core/config.h"
#include "Windows.h"

namespace NAMESPACE_FLUENTX {
	class WindowsConstants {
	public:
		WindowsConstants();
		~WindowsConstants();

		static HINSTANCE& getHinstance();
	private:
		static HINSTANCE mHInstance;
	};
}