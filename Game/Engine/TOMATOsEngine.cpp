#include "TOMATOsEngine.h"

#include <cassert>

#include "GameWindow.h"
#include "RenderManager.h"
#include "TriangleRenderer.h"
#include "SpriteRenderer.h"
#include "TextureManager.h"

namespace {
    GameWindow* gameWindow = nullptr;
    RenderManager* renderManager = nullptr;
    TriangleRenderer* triangleRenderer = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    TextureManager* textureManager = nullptr;

    Matrix4x4 screenMatrix_;

}

namespace TOMATOsEngine {

    void Initialize() {
        assert(!gameWindow);
        assert(!renderManager);
        assert(!triangleRenderer);
        assert(!spriteRenderer);
        assert(!textureManager);

        gameWindow = GameWindow::GetInstance();
        gameWindow->Initialize(L"Title", 1280, 720);

        renderManager = RenderManager::GetInstance();
        renderManager->Initialize();

        triangleRenderer = TriangleRenderer::GetInstance();
        triangleRenderer->Initialize(renderManager->GetMainBufferRTVFormat());

        spriteRenderer = SpriteRenderer::GetInstance();
        spriteRenderer->Initialize(renderManager->GetMainBufferRTVFormat());

        textureManager = TextureManager::GetInstance();

        screenMatrix_ = Matrix4x4::MakeOrthographicProjection(float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
        screenMatrix_ *= Matrix4x4::MakeScaling({ 1.0f, 1.0f,1.0f });
        screenMatrix_ *= Matrix4x4::MakeTranslation({ -1.0f, -1.0f, 0.0f });

        renderManager->BeginRender();
    }

    void Shutdown() {
        renderManager->Shutdown();
        gameWindow->Shutdown();
    }

    bool BeginFrame() {

        renderManager->EndRender();

        if (!gameWindow->ProcessMessage()) {
            return false;
        }

        renderManager->BeginRender();
        triangleRenderer->Reset();
        spriteRenderer->Reset();

        return true;
    }

    TextureHandle LoadTexture(const std::string& name) {
        return textureManager->Load(name);
    }

    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color, FillMode fillMode) {
        TriangleRenderer::Vertex vertices[] = {
           { Vector3(pos0) * screenMatrix_, color },
           { Vector3(pos1) * screenMatrix_, color },
           { Vector3(pos2) * screenMatrix_, color },
        };

        if (fillMode == kFillModeSolid) {

            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices, 3);
        }
    }

    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color, FillMode fillMode) {
        TriangleRenderer::Vertex vertices[] = {
            { { min.x, max.y, 0.0f }, color },
            { { min.x, min.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { min.x, max.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { max.x, max.y, 0.0f }, color },
        };

        for (auto& vertex : vertices) {
            vertex.position = vertex.position * screenMatrix_;
        }

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices, 2);
        }
    }

    void DrawRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, uint32_t color, FillMode fillMode) {
        Vector2 tmp[] = {
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix_).GetXY();
        }

        TriangleRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color },
            { {tmp[1], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[0], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[3], 0.0f}, color },
        };

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices, 2);
        }
    }



    void DrawCircle(const Vector2& pos, float radius, uint32_t color, FillMode fillMode) {
        constexpr uint32_t kNumSubdivisions = 16;
        constexpr uint32_t kNumTriangles = kNumSubdivisions - 2;

        struct UnitCircle {
            UnitCircle() {
                float every = Math::TwoPi / float(kNumSubdivisions);
                for (uint32_t i = 0; i < kNumSubdivisions; ++i) {
                    float angle = every * i;
                    vertices[i].x = std::cos(angle);
                    vertices[i].y = std::sin(angle);
                }
            }
            Vector2 vertices[kNumSubdivisions];
        };
        static const UnitCircle unitCircle;

        Matrix3x3 matrix{
            radius, 0.0f, 0.0f,
            0.0f, radius, 0.0f,
            pos.x, pos.y, 1.0f };

        Vector3 global[kNumSubdivisions]{};
        for (uint32_t i = 0; i < kNumSubdivisions; ++i) {
            global[i] = Vector3(unitCircle.vertices[i] * matrix) * screenMatrix_;
        }

        TriangleRenderer::Vertex vertices[kNumTriangles][3]{};
        for (uint32_t i = 0; i < kNumTriangles; ++i) {
            vertices[i][0].position = global[0];
            vertices[i][0].color = color;
            vertices[i][1].position = global[i + 1];
            vertices[i][1].color = color;
            vertices[i][2].position = global[i + 2];
            vertices[i][2].color = color;
        }

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices[0], kNumTriangles);
        }
    }

    void DrawSpriteRect(const Vector2& min, const Vector2& max, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        assert(texHandle.IsValid());

        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        SpriteRenderer::Vertex vertices[] = {
        { { min.x, max.y, 0.0f }, color, {ul, ut} },
        { { min.x, min.y, 0.0f }, color, {ul, ub} },
        { { max.x, min.y, 0.0f }, color, {ur, ub} },
        { { min.x, max.y, 0.0f }, color, {ul, ut} },
        { { max.x, min.y, 0.0f }, color, {ur, ub} },
        { { max.x, max.y, 0.0f }, color, {ur, ut} },
        };

        for (auto& vertex : vertices) {
            vertex.position = vertex.position * screenMatrix_;
        }

        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawSpriteRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        assert(texHandle.IsValid());

        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        Vector2 tmp[] = {
           { 0.0f, 0.0f },
           { 0.0f, 1.0f },
           { 1.0f, 1.0f },
           { 1.0f, 0.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix_).GetXY();
        }

        SpriteRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color, {ul, ut} },
            { {tmp[1], 0.0f}, color, {ul, ub} },
            { {tmp[2], 0.0f}, color, {ur, ub} },
            { {tmp[0], 0.0f}, color, {ul, ut} },
            { {tmp[2], 0.0f}, color, {ur, ub} },
            { {tmp[3], 0.0f}, color, {ur, ut} },
        };
        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawSpriteQuad(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        Vector2 tmp[] = {
           lb,
           lt,
           rt,
           rb,
        };

        for (auto& vertex : tmp) {
            vertex = (Vector3(vertex) * screenMatrix_).GetXY();
        }

        SpriteRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[1], 0.0f}, color, {ul, ut} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[3], 0.0f}, color, {ur, ub} },
        };
        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

}
