#define NOMINMAX
#include <Windows.h>

#include "Engine/GameWindow.h"
#include "Engine/RenderManager.h"
#include "Engine/TextureManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    auto gameWindow = GameWindow::GetInstance();
    gameWindow->Initialize(L"Title", 1280, 720);

    auto renderManager = RenderManager::GetInstance();
    renderManager->Initialize();

    TextureManager::GetInstance()->Load("Resources/uvChecker.png");

    while (gameWindow->ProcessMessage()) {

        renderManager->BeginRender();


        renderManager->EndRender();
    }
    renderManager->Shutdown();
    gameWindow->Shutdown();

    return 0;
}