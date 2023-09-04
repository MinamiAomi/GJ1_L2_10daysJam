#pragma once

#include <Windows.h>

#include <string>


namespace Utility {

    std::wstring ConvertString(const std::string& str);
    std::string ConvertString(const std::wstring& str);
    
    void AssertIfFailed(HRESULT hr, const wchar_t* str);

}

#ifndef MY_DEFINES
#define MY_DEFINES

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WIDE_(x) L##x
#define WIDE(x) WIDE_(x)

#define ASSERT_IF_FAILED(hr) Utility::AssertIfFailed(hr, WIDE(__FILE__) "(" STRINGIFY(__LINE__) ")\n" STRINGIFY(hr))

#endif // MY_DEFINES