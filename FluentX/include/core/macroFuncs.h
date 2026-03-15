#pragma once
#include "config.h"
#include "utility.h"
#include <stdexcept>
#include <string>
#include <locale>
#include <codecvt>

#ifdef UNICODE
/*
Parameter : Expected a std::string error message
*/
#define FLUENTX_THROW_ERROR(x) do { \
    if (FLUENTX_ENABLE_MSGBOX_ERRORS == 1) { \
        std::wstring wmsg = StringToWString(x); \
        MessageBoxW(nullptr, wmsg.c_str(), L"FluentX Runtime Error", MB_ICONERROR | MB_OK); \
    } \
    if (FLUENTX_ENABLE_RUNTIME_ERRORS == 1) { \
        throw std::runtime_error(x); \
    } \
} while(0)
#else
/*
Parameter : Expected a std::string error message
*/
#define FLUENTX_THROW_ERROR(x) do { \
    if (FLUENTX_ENABLE_MSGBOX_ERRORS == 1) { \
        MessageBoxA(nullptr, x.c_str(), "FluentX Runtime Error", MB_ICONERROR | MB_OK); \
    } \
    if (FLUENTX_ENABLE_RUNTIME_ERRORS == 1) { \
        throw std::runtime_error(x); \
    } \
} while(0)
#endif
