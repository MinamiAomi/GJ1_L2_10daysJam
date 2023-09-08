#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "BackGround.h"
#include "Field.h"
#include "Player.h"

#include "RenderManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    TOMATOsEngine::Initialize();

    auto tex = TOMATOsEngine::LoadTexture("Resources/playgame.png");

    BackGround backGround;
    backGround.Initialize();

    Field field;
    field.Initialize();

    Player player;
    player.Initialize();
    player.SetField(&field);
    player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });

    while (TOMATOsEngine::BeginFrame()) {

        if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
            field.Initialize();
            player.Initialize();
            player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
        }

        field.Update();
        backGround.Update();
        player.Update();


        field.Draw();
        player.Draw();
        backGround.Draw();

        auto r = RenderManager::GetInstance();
        ImGui::Begin("Bloom");
        static float t = 0.0f;
        ImGui::DragFloat("Threshold", &t, 0.01f, 0.0f, 1.0f);
        r->GetBloom().SetThreshold(t);
        static float k = 0.0f;
        ImGui::DragFloat("Knee", &k, 0.01f, 0.0f, 1.0f);
        r->GetBloom().SetKnee(k);
        ImGui::End();

        field.Edit();
    }

    TOMATOsEngine::Shutdown();

    return 0;
}