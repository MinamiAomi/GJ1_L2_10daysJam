#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "BackGround.h"
#include "Field.h"
#include "Particle/ParticleManager.h"
#include "Player.h"

#include "RenderManager.h"

#include "Audio/Audio.h"

#include "GameTime.h"
#include "Math/Color.h"

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
    TextureHandle gameOverHandle = TOMATOsEngine::LoadTexture("Resources/gameOver.png");
    Vector2 gameOverPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };
    Vector2 gameOverSize = { 320.0f,240.0f };
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

    while (TOMATOsEngine::BeginFrame()) {



        switch (gameScene)
        {
        case title:

            if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
                gameScene = inGame;
                backGround.Initialize();
                particleManager.Initialize();
                field.Initialize();
                player.Initialize();
                player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
            }

            TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, titleHandle, 0xFFFFFFFF);

            break;
        case inGame:
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
            if (!field.GetIsInGameOver()) {
                gameTime->Update();
            }
            

            backGround.Draw();
            field.Draw();
            particleManager.Draw();
            player.Draw();
            if (!field.GetIsInGameOver()) {
                gameTime->Draw();
            }
            

            field.Edit();
            break;
        case gameOver:
            if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
                gameScene = title;
            }
            backGround.Update();
            backGround.Draw();
            TOMATOsEngine::DrawSpriteRectAngle(gameOverPosition, gameOverSize, { 0.5f,0.5f }, 0.0f, {0.0f,0.0f}, gameOverSize, gameOverHandle,Color(0.5f, 0.5f, 0.5f, 0.5f));
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