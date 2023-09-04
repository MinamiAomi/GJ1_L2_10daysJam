#include "Utility.h"

#include <format>

namespace Utility {

    std::wstring ConvertString(const std::string& str) {
        if (str.empty()) {
            return std::wstring();
        }
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
        if (sizeNeeded == 0) {
            return std::wstring();
        }
        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
        return result;
    }

    std::string ConvertString(const std::wstring& str) {
        if (str.empty()) {
            return std::string();
        }
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
        if (sizeNeeded == 0) {
            return std::string();
        }
        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
        return result;
    }

    void AssertIfFailed(HRESULT hr, const wchar_t* str) {
        if (FAILED(hr)) {
            MessageBoxW(nullptr, str, L"HRESUT FAILED", S_OK);
            OutputDebugStringW(std::format(L"\n/////HRESULT FAILED/////\n{} = {}\n/////HRESULT FAILED/////\n\n", str, hr).c_str());
            std::exit(EXIT_FAILURE);
        }
    }

}