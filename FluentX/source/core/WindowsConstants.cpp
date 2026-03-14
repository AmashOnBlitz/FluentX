#include "core/WindowsConstants.h"
#include "core/macroFuncs.h"
#include <string>

HINSTANCE NAMESPACE_FLUENTX::WindowsConstants::mHInstance = NULL;

NAMESPACE_FLUENTX::WindowsConstants::WindowsConstants()
{
}


NAMESPACE_FLUENTX::WindowsConstants::~WindowsConstants()
{
}

HINSTANCE& NAMESPACE_FLUENTX::WindowsConstants::getHinstance()
{
	if (mHInstance == INVALID_HANDLE_VALUE || mHInstance == NULL) {
		mHInstance = GetModuleHandle(NULL);
		if (mHInstance == INVALID_HANDLE_VALUE || mHInstance == NULL) {
			std::string err = "GetModuleHandle failed. Error: " + std::to_string(GetLastError());
			FLUENTX_THROW_ERROR(err);
		}
	}
	return mHInstance;
}
