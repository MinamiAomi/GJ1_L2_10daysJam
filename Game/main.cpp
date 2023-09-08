#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "Field.h"
#include "Player.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    TOMATOsEngine::Initialize();

    auto tex = TOMATOsEngine::LoadTexture("Resources/uvChecker.png");

    Field field;
    field.Initialize();

    Player player;
    player.Initialize();
    player.SetField(&field);
    player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });

    while (TOMATOsEngine::BeginFrame()) {

        if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
            player.Initialize();
            player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
        }

        field.Update();
        player.Update();

        field.Draw();
        player.Draw();
    }

    TOMATOsEngine::Shutdown();

    return 0;
}