#pragma once

#include <Windows.h>

#include <cstdint>
#include <string>

class Window {
public:
    static Window& Get();

    void Create(const std::string& name, uint32_t clientWidth, uint32_t clientHeight);
    void Destroy();
    bool ProcessMessage() const;

    HWND GetHWND() const { return hWnd_; }
    uint32_t GetClientWidth() const { return clientWidth_; }
    uint32_t GetClientHeight() const { return clientHeight_; }

private:
    Window() = default;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    HWND hWnd_;
    uint32_t clientWidth_;
    uint32_t clientHeight_;

};