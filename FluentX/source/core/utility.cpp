#include "core/pch.h"
#include "core/utility.h"

std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        (int)wstr.size(),
        nullptr,
        0,
        nullptr, nullptr
    );

    std::string strTo(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        (int)wstr.size(),
        strTo.data(),
        size_needed,
        nullptr, nullptr
    );

    return strTo;
}

std::wstring StringToWString(const std::string& str)
{
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), wstrTo.data(), size_needed);
    return wstrTo;
}

std::string GetLocalAppDataPath()
{
    char exePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) == 0)
        return ".\\";

    std::filesystem::path path(exePath);
    std::string exeName = path.stem().string();

    char localAppData[MAX_PATH] = { 0 };
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, localAppData)))
    {
        std::filesystem::path finalPath = std::filesystem::path(localAppData) / exeName;
        std::filesystem::create_directories(finalPath);
        return std::string(finalPath.string() + "\\FXData");
    }

    return ".\\";
}