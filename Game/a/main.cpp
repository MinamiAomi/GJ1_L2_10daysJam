#include <Windows.h>

#include "Core/Window.h"
#include "Core/Graphics.h"
#include "Core/ImGuiManager.h"
#include "Core/ShaderCompiler.h"

#include "Engine/RenderManager.h"
#include "Engine/Input.h"

#include "GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    auto& window = Window::Get();
    window.Create("草原の主", 1280, 720);
    Input::Get().Create();
    auto& graphics = Graphics::Get();
    graphics.Create();
    auto& imguiManager = ImGuiManager::Get();
    imguiManager.Create();
    ShaderCompiler::Get().Create();
    auto& renderManager = RenderManager::Get();
    renderManager.Create();

    GameScene gameScene;
    gameScene.Initialize();

    while (window.ProcessMessage()) {
        Input::Get().Update();
        imguiManager.NewFrame();

        gameScene.Update();

        gameScene.Draw();

        renderManager.Render();

        gameScene.EndFrame();
    }

    imguiManager.Destroy();
    window.Destroy();

    return 0;
}