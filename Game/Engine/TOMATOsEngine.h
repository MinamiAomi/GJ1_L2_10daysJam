#pragma once

#include <cstdint>
#include <string>

#include "Externals/ImGui/imgui.h"

#include "Math/MathUtils.h"

#include "TextureHandle.h"
#include "Input.h"

enum BlendMode {
    kBlendModeDisable,
    kBlendModeNormal,
    kBlendModeAdditive,
    kBlendModeSubtract
};

namespace TOMATOsEngine {

    static uint32_t kWindowWidth = 1280;
    static uint32_t kWindowHeight = 720;
    static uint32_t kMonitorWidth = 640;
    static uint32_t kMonitorHeight = 480;

    void Initialize();
    void Shutdown();

    bool BeginFrame();

    TextureHandle LoadTexture(const std::string& name);


    void SetBlendMode(BlendMode blendMode);
    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color);
    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color);
    void DrawRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, uint32_t color);
    void DrawCircle(const Vector2& pos, float radius, uint32_t color);
    void DrawSpriteRect(const Vector2& min, const Vector2& max, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);
    void DrawSpriteRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);
    void DrawSpriteQuad(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);

    bool IsKeyPressed(char keycode);
    bool IsKeyTrigger(char keycode);
    bool IsKeyRelease(char keycode);

    bool IsMousePressed(int button);
    bool IsMouseTrigger(int button);
    bool IsMouseRelease(int button);
    Vector2 GetMousePosition();
    Vector2 GetMouseMove();
    float GetMouseWheel();
    const XINPUT_STATE& GetGamePadState();
    const XINPUT_STATE& GetGamePadPreState();

}