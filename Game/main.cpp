#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "BackGround.h"
#include "Field.h"
#include "FeverManager.h"
#include "Particle/ParticleManager.h"
#include "Player.h"

#include "RenderManager.h"

#include "Audio/Audio.h"

#include "Math/Color.h"
#include "LevelManager.h"

static float pitch = 1.0f;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	TOMATOsEngine::Initialize();
#ifdef _DEBUG
	TOMATOsEngine::SetFullScreen(false);
#endif // _DEBUG


	enum GameScene {
		title,
		inGame,
		gameClear,
	};

	GameScene gameScene = title;

	TextureHandle titleHandle = TOMATOsEngine::LoadTexture("Resources/BBtitle.png");
	TextureHandle gameOverHandle = TOMATOsEngine::LoadTexture("Resources/gameOver.png");
	TextureHandle floorHandle = TOMATOsEngine::LoadTexture("Resources/floor.png");
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

	field.SetPlayer(&player);

	BackGround backGround;
	backGround.Initialize();
	backGround.SetPlayer(&player);

	LevelManager levelManager;
	levelManager.GetFild(&field);
	levelManager.Initialize();

	FeverManager* feverManager = FeverManager::GetInstance();

	auto pushSpaceSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pushSpace.wav");
	auto titleSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/titleBGM.wav");
	auto ingameSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/ingameBGM.wav");
	auto clearSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/clearBGM.wav");

	// タイトルははじめから流す
	size_t titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
	TOMATOsEngine::SetVolume(titlePlayHandle, 0.2f);
	size_t ingamePlayHandle = 0;
	size_t clearPlayHandle = 0;

    //bool isFullScreen = false;
	// 音の溜め必要
	bool ingameToClear = false;
	bool clearToTitle = false;

	while (TOMATOsEngine::BeginFrame()) {

#ifdef _DEBUG
		auto& io = ImGui::GetIO();
		ImGui::Begin("Menu");
		ImGui::Text("FPS : %f\n", io.Framerate);
		ImGui::Text("Quit : ESCAPE\n");
		ImGui::Text("FullScreen : TAB\n");
		ImGui::End();
#endif // _DEBUG


		switch (gameScene) {
		case title:
		{
			if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
				gameScene = inGame;
				backGround.Initialize();
				particleManager.Initialize();
				field.Initialize();
				player.Initialize();
				player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
				feverManager->Initialize();
				// 音
				// タイトルBGM停止
				TOMATOsEngine::StopAudio(titlePlayHandle);
				// インゲームBGM
				ingamePlayHandle = TOMATOsEngine::PlayAudio(ingameSoundHandle, true);
				// スペース押した音
				auto pushSpacePlayHandle = TOMATOsEngine::PlayAudio(pushSpaceSoundHandle);
				TOMATOsEngine::SetVolume(pushSpacePlayHandle, 0.1f);
			}

			TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, titleHandle, 0xFFFFFFFF);

			break;
		}
		case inGame:
		{
			// 音
			if (field.GetIsVanish() == true) {
				gameScene = gameClear;
			}
			field.Update();
			backGround.Update();
			player.Update();

			particleManager.Update();
			levelManager.Update();
			feverManager->Update();

			if (!field.GetIsInGameOver()) {
				TOMATOsEngine::DrawSpriteRectCenter({ TOMATOsEngine::kMonitorWidth * 0.5f , TOMATOsEngine::kMonitorHeight * 0.5f - 10.0f }, { TOMATOsEngine::kMonitorWidth * 1.0f , TOMATOsEngine::kMonitorHeight * 1.0f }, { 0.0f,0.0f }, { TOMATOsEngine::kMonitorWidth * 1.0f , TOMATOsEngine::kMonitorHeight * 1.0f }, floorHandle, 0xFFFFFFFF);
				feverManager->Draw();
				backGround.Draw();
				particleManager.Draw();
				player.ComboDraw();
				levelManager.Draw();
			}
			else {
				backGround.FrameDraw();
				// 音
				if (!ingameToClear) {
					// インゲームBGM停止
					TOMATOsEngine::StopAudio(ingamePlayHandle);
					ingameToClear = true;
				}
			}
			field.Draw();
			player.Draw();

#ifdef _DEBUG
			field.Edit();
#endif // _DEBUG
			break;
		}
		case gameClear:
		{
			player.Update();
			backGround.FrameDraw();
			player.Draw();
			if (player.GetIsEndGameClearEasing()) {
				field.DrawScore();
				if (!clearToTitle) {
					// ゲームクリアBGM
					clearPlayHandle = TOMATOsEngine::PlayAudio(clearSoundHandle, true);
					clearToTitle = true;
				}
			}
			if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
				gameScene = title;
				backGround.Initialize();
				particleManager.Initialize();
				field.Initialize();
				player.Initialize();
				player.SetPosition({ field.GetSize().x * 0.5f, field.GetSize().y - 100.0f });
				levelManager.Initialize();
				// 音
				// クリアBGM停止
				TOMATOsEngine::StopAudio(clearPlayHandle);
				// タイトルBGM
				titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
				// スペースオン
				auto pushSpacePlayHandle = TOMATOsEngine::PlayAudio(pushSpaceSoundHandle);
				TOMATOsEngine::SetVolume(pushSpacePlayHandle, 0.1f);
				clearToTitle = false;
				ingameToClear = false;
			}
			break;
		}

		default:
		{
			break;
		}
		}
		// 3Dカメラを変更
		if (TOMATOsEngine::IsKeyTrigger(DIK_TAB)) {
			TOMATOsEngine::SwitchViewMode();
		}
		// ゲームを終了
		if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
			TOMATOsEngine::RequestQuit();
		}
	}

	TOMATOsEngine::Shutdown();

	return 0;
}