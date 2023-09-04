#define NOMINMAX
#include <Windows.h>

#include "Engine/GameWindow.h"
#include "Engine/RenderManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    auto gameWindow = GameWindow::GetInstance();
    gameWindow->Initialize(L"Title", 1280, 720);

    auto renderManager = RenderManager::GetInstance();
    renderManager->Initialize(gameWindow->GetHWND());


    while (gameWindow->ProcessMessage()) {

        renderManager->Render();

    }

    gameWindow->Terminate();

    return 0;
}