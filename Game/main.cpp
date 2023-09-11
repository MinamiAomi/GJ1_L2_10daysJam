#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "BackGround.h"
#include "Field.h"
#include "Particle/ParticleManager.h"
#include "Player.h"

#include "RenderManager.h"

#include "Audio/Audio.h"

#include "GameTime.h"

static float pitch = 1.0f;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    TOMATOsEngine::Initialize();


    enum GameScene {
        title,
        inGame,
        gameClear,
        gameOver,
    };

    GameScene gameScene = title;

    auto tex = TOMATOsEngine::LoadTexture("Resources/playgame.png");
    TextureHandle titleHandle = TOMATOsEngine::LoadTexture("Resources/BBtitle.png");

    ParticleManager particleManager;
    particleManager.Initialize();

    Field field;
    field.Initialize();
    field.SetParticleManager(&particleManager);

    Player player;
    player.Initialize();
    player.SetField(&field);
    player.SetParticleManager(&particleManager);
    player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });

    BackGround backGround;
    backGround.Initialize();
    backGround.SetPlayer(&player);

    GameTime* gameTime = GameTime::GetInstance();

    size_t bgmSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/ingameBGM.wav");
    size_t bgmPlayHandle = 0;

    while (TOMATOsEngine::BeginFrame()) {



        switch (gameScene)
        {
        case title:

            if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
                gameScene = inGame;
                bgmPlayHandle = TOMATOsEngine::PlayAudio(bgmSoundHandle, true);
                TOMATOsEngine::SetVolume(bgmPlayHandle, 0.9f);
            }

            TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, titleHandle, 0xFFFFFFFF);

            break;
        case inGame:
            ImGui::Begin("BGM");
            ImGui::Text("Z button");
            ImGui::DragFloat("Pitch", &pitch, 0.01f);
            ImGui::End();
            if (TOMATOsEngine::IsKeyTrigger(DIK_Z)) {
                TOMATOsEngine::SetPitch(bgmPlayHandle, pitch);
            }

            if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
                field.Initialize();
                player.Initialize();
                player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
            }
            if (field.GetIsGameOver()) {
                gameScene = gameOver;
            }

            field.Update();
            backGround.Update();
            player.Update();
            particleManager.Update();
            gameTime->Update();

            gameTime->time_

            backGround.Draw();
            field.Draw();
            particleManager.Draw();
            player.Draw();
            gameTime->Draw();


            field.Edit();
            break;
        case gameOver:
            break;
        case gameClear:
            break;
        default:
            break;
        }

    }

    TOMATOsEngine::Shutdown();

    return 0;
}