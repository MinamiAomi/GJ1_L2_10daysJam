#pragma once

#include <Windows.h>
#include <cstdint>

class GameWindow {
public:
    static GameWindow* GetInstance();

    void Initialize(const wchar_t* title, uint32_t clientWidth, uint32_t clientHeight);
    bool ProcessMessage() const;
    void Shutdown();

    HWND GetHWND() const { return hWnd_; }
    uint32_t GetClientWidth() const { return clientWidth_; }
    uint32_t GetClientHeight() const { return clientHeight_; }

private:
    GameWindow() = default;
    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    HWND hWnd_;
    uint32_t clientWidth_;
    uint32_t clientHeight_;
};