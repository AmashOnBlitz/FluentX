#pragma once
#include <string>
#include <Windows.h>

std::string WStringToString(const std::wstring& wstr);
std::wstring StringToWString(const std::string& str);
std::string GetLocalAppDataPath();