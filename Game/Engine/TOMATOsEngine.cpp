#include "TOMATOsEngine.h"

#include <cassert>

#include "GameWindow.h"
#include "RenderManager.h"
#include "TriangleRenderer.h"
#include "TextureManager.h"

namespace {
    GameWindow* gameWindow = nullptr;
    RenderManager* renderManager = nullptr;
    TriangleRenderer* triangleRenderer = nullptr;
    TextureManager* textureManager = nullptr;

    Matrix4x4 screenMatrix_;

}

namespace TOMATOsEngine {

    void Initialize() {
        assert(!gameWindow);
        assert(!renderManager);
        assert(!triangleRenderer);
        assert(!textureManager);

        gameWindow = GameWindow::GetInstance();
        gameWindow->Initialize(L"Title", 1280, 720);

        renderManager = RenderManager::GetInstance();
        renderManager->Initialize();

        triangleRenderer = TriangleRenderer::GetInstance();
        triangleRenderer->Initialize(renderManager->GetMainBufferRTVFormat());

        textureManager = TextureManager::GetInstance();

        screenMatrix_ = Matrix4x4::MakeOrthographicProjection(float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
        screenMatrix_ *= Matrix4x4::MakeScaling({ 1.0f, -1.0f,1.0f });
        screenMatrix_ *= Matrix4x4::MakeTranslation({ -1.0f, 1.0f, 0.0f });

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

        return true;
    }

    uint32_t LoadTexture(const std::string& name) {
        return static_cast<uint32_t>(textureManager->Load(name).GetIndex());
    }

    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color, FillMode fillMode) {

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                Vector3(pos0) * screenMatrix_,
                Vector3(pos1) * screenMatrix_,
                Vector3(pos2) * screenMatrix_,
                color);
        }
    }

    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color, FillMode fillMode) {
        Vector3 vertices[] = {
            { min.x, max.y, 0.0f },
            { min.x, min.y, 0.0f },
            { max.x, min.y, 0.0f },
            { min.x, max.y, 0.0f },
            { max.x, min.y, 0.0f },
            { max.x, max.y, 0.0f },
        };

        for (auto& pos : vertices) {
            pos = pos * screenMatrix_;
        }

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices, 2, color);
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

        Vector3 vertices[] = {
            { tmp[0], 0.0f },
            { tmp[1], 0.0f },
            { tmp[2], 0.0f },
            { tmp[0], 0.0f },
            { tmp[2], 0.0f },
            { tmp[3], 0.0f },
        };

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices, 2, color);
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

        Vector3 vertices[kNumTriangles][3]{};
        for (uint32_t i = 0; i < kNumTriangles; ++i) {
            vertices[i][0] = global[0];
            vertices[i][1] = global[i + 1];
            vertices[i][2] = global[i + 2];
        }

        if (fillMode == kFillModeSolid) {
            triangleRenderer->Draw(
                renderManager->GetCommandContext(),
                vertices[0], kNumTriangles, color);
        }
    }

}
