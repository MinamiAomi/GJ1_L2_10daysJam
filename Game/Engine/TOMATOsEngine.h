#pragma once

#include <cstdint>
#include <string>

#include "Externals/ImGui/imgui.h"

#include "Math/MathUtils.h"

#include "TextureHandle.h"

enum BlendMode {
    kBlendModeDisable,
    kBlendModeNormal,
    kBlendModeAdditive,
    kBlendModeSubtract
};

enum FillMode {
    kFillModeSolid,
    kFillModeWireframe
};

namespace TOMATOsEngine {

    static uint32_t kWindowWidth = 1280;
    static uint32_t kWindowHeight = 720;

    struct Triangle {
        Vector2 p0, p1, p2;
    };
    struct Rect {
        Vector2 min, max;
    };
    struct RectAngle {
        Vector2 center;
        Vector2 size;
        float radius;
    };
    struct Circle {
        Vector2 center;
        float radius;
    };
    struct Quad {
        Vector2 p0, p1, p2, p3;
    };

    void Initialize();
    void Shutdown();

    bool BeginFrame();

    TextureHandle LoadTexture(const std::string& name);

    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color, FillMode fillMode = kFillModeSolid);
    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color, FillMode fillMode = kFillModeSolid);
    void DrawRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, uint32_t color, FillMode fillMode = kFillModeSolid);
    void DrawCircle(const Vector2& pos, float radius, uint32_t color, FillMode fillMode = kFillModeSolid);
    void DrawSpriteRect(const Vector2& min, const Vector2& max, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);
    void DrawSpriteRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);
    void DrawSpriteQuad(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color);
}