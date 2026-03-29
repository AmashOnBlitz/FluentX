#pragma once
#include <string>
#include <Windows.h>
#include "core/config.h"

std::string WStringToString(const std::wstring& wstr);
std::wstring StringToWString(const std::string& str);
std::string GetLocalAppDataPath();

void DebugCoutLog(std::string msg);