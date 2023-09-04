#define NOMINMAX
#include <Windows.h>

#include "Engine/Application/GameWindow.h"
#include "Engine/Graphics/Graphics.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    GameWindow* gameWindow = GameWindow::GetInstance();
    gameWindow->Initialize(L"Title", 1280, 720);

    Graphics* graphics = Graphics::GetInstance();
    graphics->Initialize();

    while (gameWindow->ProcessMessage()) {

        // コラボお願いします
        // a
    }

    gameWindow->Terminate();

    return 0;
}