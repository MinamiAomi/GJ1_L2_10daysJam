#include "TOMATOsEngine.h"

#include <cassert>
#include <memory>
#include <chrono>
#include <thread>

#include "GameWindow.h"
#include "RenderManager.h"
#include "TriangleRenderer.h"
#include "SpriteRenderer.h"
#include "TextureManager.h"
#include "Model.h"
#include "RealWorld.h"
#include "Monitor.h"

#include "Audio/Audio.h"

namespace {
    GameWindow* gameWindow = nullptr;
    RenderManager* renderManager = nullptr;
    TriangleRenderer* triangleRenderer = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    TextureManager* textureManager = nullptr;
    Input* input = nullptr;
    Audio* audio = nullptr;

    Matrix4x4 screenMatrix;
    Matrix4x4 frameMatrix;

    Monitor* monitor = nullptr;
    std::unique_ptr<RealWorld> realWorld;
    RealWorld::ViewMode viewMode;

    std::chrono::steady_clock::time_point referenceTime;

    bool isEndRequest = false;

    inline uint32_t RGBAtoABGR(uint32_t argb) {
        uint8_t R = (argb >> 24) & 0xFFu;
        uint8_t G = (argb >> 16) & 0xFFu;
        uint8_t B = (argb >> 8) & 0xFFu;
        uint8_t A = (argb >> 0) & 0xFFu;
        return (A << 24) | (B << 16) | (G << 8) | (R << 0);
    }
}

namespace TOMATOsEngine {

    void Initialize() {
        assert(!gameWindow);
        assert(!renderManager);
        assert(!triangleRenderer);
        assert(!spriteRenderer);
        assert(!textureManager);
        assert(!input);
        assert(!monitor);
        assert(!audio);

        gameWindow = GameWindow::GetInstance();
        gameWindow->Initialize(L"2006_BB", 1280, 720);

        input = Input::GetInstance();
        input->Initialize(gameWindow->GetHWND());

        renderManager = RenderManager::GetInstance();
        renderManager->Initialize();

        Model::CreatePipeline(renderManager->GetMainBufferRTVFormat(), renderManager->GetMainDepthDSVFormat());

        monitor = Monitor::GetInstance();
        monitor->Initilaize(kMonitorWidth, kMonitorHeight, renderManager->GetMainBufferRTVFormat(), renderManager->GetMainDepthDSVFormat());

        realWorld = std::make_unique<RealWorld>();
        realWorld->Initialize();

        triangleRenderer = TriangleRenderer::GetInstance();
        triangleRenderer->Initialize(monitor->GetColorBuffer().GetFormat());

        spriteRenderer = SpriteRenderer::GetInstance();
        spriteRenderer->Initialize(monitor->GetColorBuffer().GetFormat());

        textureManager = TextureManager::GetInstance();

        audio = Audio::GetInstance();
        audio->Initialize();

        gameWindow->SetFullScreen(true);

        frameMatrix = Matrix4x4::MakeTranslation({ 0.0f, 40.0f, 0.0f });
        frameMatrix *= Matrix4x4::MakeOrthographicProjection(float(kMonitorWidth), float(kMonitorHeight), 0.0f, 1.0f);
        frameMatrix *= Matrix4x4::MakeScaling({ 1.0f, 1.0f,1.0f });
        frameMatrix *= Matrix4x4::MakeTranslation({ -1.0f, -1.0f, 0.0f });

        screenMatrix = frameMatrix * Matrix4x4::MakeScaling({ 0.93f, 0.91f, 0.0f });

        renderManager->Reset();

        renderManager->BeginRender();

        isEndRequest = false;

        referenceTime = std::chrono::steady_clock::now();
    }

    void Shutdown() {
        renderManager->Shutdown();
        realWorld.reset();
        Model::DestroyPipeline();
        gameWindow->Shutdown();
    }

    bool BeginFrame() {
        realWorld->Update();

        renderManager->BeginRender();
        realWorld->Draw(renderManager->GetCommandContext());
        renderManager->EndRender();

        // FPS固定
        {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::chrono::microseconds elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(now - referenceTime);

            static const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 62.0f));
            static const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
            std::chrono::microseconds check = kMinCheckTime - elapsed;
            if (check > std::chrono::microseconds(0)) {
                std::chrono::microseconds waitTime = kMinTime - elapsed;

                std::chrono::steady_clock::time_point waitStart = std::chrono::steady_clock::now();
                do {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                } while (std::chrono::steady_clock::now() - waitStart < waitTime);
            }

            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - referenceTime);
            referenceTime = std::chrono::steady_clock::now();
        }

        if (!gameWindow->ProcessMessage() ||
            isEndRequest) {
            return false;
        }

        input->Update();
        audio->Update();

        renderManager->Reset();

        monitor->BeginRender(renderManager->GetCommandContext());


        return true;
    }

    TextureHandle LoadTexture(const std::string& name) {
        return textureManager->Load(name);
    }

    void SetBlendMode(BlendMode blendMode) {
        switch (blendMode)
        {
        case kBlendModeDisable:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendDisable);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendDisable);
            break;
        case kBlendModeNormal:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendNormal);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendNormal);
            break;
        case kBlendModeAdditive:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendAdditive);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendAdditive);
            break;
        case kBlendModeSubtract:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendSubtract);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendSubtract);
            break;
        default:
            assert(false);
            break;
        }

    }

    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color) {
        color = RGBAtoABGR(color);

        TriangleRenderer::Vertex vertices[] = {
           { Vector3(pos0) * screenMatrix, color },
           { Vector3(pos1) * screenMatrix, color },
           { Vector3(pos2) * screenMatrix, color },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 3);
    }

    void DrawTriangle(const Vector2& p0, uint32_t c0, const Vector2& p1, uint32_t c1, const Vector2& p2, uint32_t c2) {
        c0 = RGBAtoABGR(c0);
        c1 = RGBAtoABGR(c1);
        c2 = RGBAtoABGR(c2);

        TriangleRenderer::Vertex vertices[] = {
           { Vector3(p0) * screenMatrix, c0 },
           { Vector3(p1) * screenMatrix, c1 },
           { Vector3(p2) * screenMatrix, c2 },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 3);
    }

    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color) {
        color = RGBAtoABGR(color);
        TriangleRenderer::Vertex vertices[] = {
            { { min.x, max.y, 0.0f }, color },
            { { min.x, min.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { min.x, max.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { max.x, max.y, 0.0f }, color },
        };

        for (auto& vertex : vertices) {
            vertex.position = vertex.position * screenMatrix;
        }

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 2);
    }

    void DrawRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, uint32_t color) {
        color = RGBAtoABGR(color);
        Vector2 tmp[] = {
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
        }

        TriangleRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color },
            { {tmp[1], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[0], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[3], 0.0f}, color },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 2);
    }



    void DrawCircle(const Vector2& pos, float radius, uint32_t color) {
        color = RGBAtoABGR(color);
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
            global[i] = Vector3(unitCircle.vertices[i] * matrix) * screenMatrix;
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

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices[0], kNumTriangles);
    }

    void DrawSpriteRect(const Vector2& min, const Vector2& max, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
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
            vertex.position = vertex.position * screenMatrix;
        }

        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawSpriteRectCenter(const Vector2& center, const Vector2& size, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        Vector2 half = size * 0.5f;
        Vector2 min = center - half;
        Vector2 max = center + half;
        DrawSpriteRect(min, max, texBase, texSize, texHandle, color);
    }

    void DrawSpriteRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
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

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
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

    void DrawSpriteQuad(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
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
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
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

    void DrawFrame(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
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

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * frameMatrix).GetXY();
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

    bool IsKeyPressed(unsigned char keycode) {
        return input->IsKeyPressed(keycode);
    }

    bool IsKeyTrigger(unsigned char keycode) {
        return input->IsKeyTrigger(keycode);
    }

    bool IsKeyRelease(unsigned char keycode) {
        return input->IsKeyRelease(keycode);
    }

    bool IsMousePressed(int button) {
        return input->IsMousePressed(button);
    }

    bool IsMouseTrigger(int button) {
        return input->IsMouseTrigger(button);
    }

    bool IsMouseRelease(int button) {
        return input->IsMouseRelease(button);
    }

    Vector2 GetMousePosition() {
        auto point = input->GetMousePosition();
        return { float(point.x), float(kWindowHeight - point.y) };
    }

    Vector2 GetMouseMove() {
        return { float(input->GetMouseMoveX()), float(input->GetMouseMoveY()) };
    }

    float GetMouseWheel() {
        return float(input->GetMouseWheel());
    }

    const XINPUT_STATE& GetGamePadState() {
        return input->GetXInputState();
    }

    const XINPUT_STATE& GetGamePadPreState() {
        return input->GetPreXInputState();
    }

    size_t LoadAudio(const std::string& name) {
        return audio->SoundLoadWave(name.c_str());
    }

    size_t PlayAudio(size_t soundHandle, bool loop) {
        if (loop)
            return audio->SoundPlayLoopStart(soundHandle);
        return audio->SoundPlayWave(soundHandle);
    }

    void StopAudio(size_t playHandle) {
        audio->StopSound(playHandle);
    }

    size_t IsPlayAudio(size_t playHandle) {
        return audio->IsValidPlayHandle(playHandle);
    }
    void SetPitch(size_t soundHandle, float pitch) {
        audio->SetPitch(soundHandle, pitch);
    }
    void SetVolume(size_t soundHandle, float volume) {
        audio->SetValume(soundHandle, volume);
    }
    void SetFullScreen(bool fullScreen) {
        gameWindow->SetFullScreen(fullScreen);
    }
    void SwitchViewMode() {
        if (realWorld) {
            switch (viewMode)
            {
            default:
            case RealWorld::ViewMode::kMonitor:
                viewMode = RealWorld::ViewMode::kBoard;
                realWorld->SetViewMode(RealWorld::ViewMode::kBoard);
                break;
            case RealWorld::ViewMode::kBoard:
                viewMode = RealWorld::ViewMode::kMonitor;
                realWorld->SetViewMode(RealWorld::ViewMode::kMonitor);
                break;
            }
        }
    }
    void RequestQuit() {
        isEndRequest = true;
    }
}
