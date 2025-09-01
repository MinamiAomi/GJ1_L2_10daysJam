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
#include "Math/Animation.h"
#include "LevelManager.h"

#define INVALID_PLAY_HANDLE (size_t(-1))

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

	TextureHandle startTextureHandle = TOMATOsEngine::LoadTexture("Resources/startText.png");
	TextureHandle operationTextureHandle = TOMATOsEngine::LoadTexture("Resources/operationText.png");
	TextureHandle endTextureHandle = TOMATOsEngine::LoadTexture("Resources/endText.png");
	TextureHandle arrowTextureHandle = TOMATOsEngine::LoadTexture("Resources/arrow.png");
	TextureHandle spaceorBTextureHandle = TOMATOsEngine::LoadTexture("Resources/spaceorb.png");

	Vector2 textSize = { 64.0f * 2.5f,32.0f * 2.5f };
	Vector2 startTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - 180.0f };
	Vector2 operationTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,startTextPosition.y - textSize.y * 0.5f - 10.0f };
	Vector2 endTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,operationTextPosition.y - textSize.y * 0.5f - 10.0f };

	Vector2 spaceorBPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - 130.0f };
	Vector2 spaceorBSize = { 150.0f * 1.5f,32.0 * 1.5f };
	Vector2 arrowPosition = { startTextPosition.x - textSize.x * 0.5f - 60.0f,startTextPosition.y };
	Vector2 arrowSize = { 32.0f,32.0f };
	uint32_t arrowSetPosition = 0;
	uint32_t arrowAnimation = 0;
	uint32_t arrowColor = 0xFFFFFFFF;
	bool flag_ = false;
	bool isSwitchViewMode = false;


	Vector2 gameOverPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - textSize.y * 2.0f };
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
	size_t pickHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pick.wav");

	// タイトルははじめから流す
	size_t titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
	TOMATOsEngine::SetVolume(titlePlayHandle, 0.2f);
	size_t ingamePlayHandle = INVALID_PLAY_HANDLE;
	size_t clearPlayHandle = INVALID_PLAY_HANDLE;

	//bool isFullScreen = false;
	// 音の溜め必要
	bool ingameToClear = false;
	bool clearToTitle = false;

	// シャットダウンアニメーション
	float kMaxAnimationTime = 20.0f;
	float animationTime = 0.0f;
	bool isShutdown = false;
	TextureHandle shutdownTextureHandle = TOMATOsEngine::LoadTexture("Resources/shpere.png");
	auto shutdownSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/shutdown.wav");
	Vector2 shutdownSize{};
	Vector2 shutdownSideSize{};
	Animation::Vector2Node shutdownSizeAnimation({ { {1280.0f, 20.0f}, 0.0f}, {{20.0f, 20.0f}, 0.2f}, {{20.0f, 640.0f}, 0.4f}, {{2.0f, 10.0f}, 0.4f}, {{1280.0f, 1.0f}, 1.0f} });
	Animation::Vector2Node shutdownSideSizeAnimation({ {{},0.0f}, { { 30.0f,100.0f }, 0.2f }, { {30.0f,200.0f} ,0.4f },{{},0.4f}, { {},1.0f } });

	while (TOMATOsEngine::BeginFrame()) {


		auto pad = TOMATOsEngine::GetGamePadState();
		auto prepad = TOMATOsEngine::GetGamePadPreState();
#ifdef _DEBUG
		auto& io = ImGui::GetIO();
		ImGui::Begin("Menu");
		ImGui::Text("FPS : %f\n", io.Framerate);
		ImGui::Text("Quit : ESCAPE\n");
		ImGui::Text("FullScreen : TAB\n");
		ImGui::Text("now:%d", pad.Gamepad.sThumbLY);
		ImGui::Text("pre:%d", prepad.Gamepad.sThumbLY);
		ImGui::End();
#endif // _DEBUG
		switch (gameScene) {
		case title:
		{
			if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
				isShutdown = true;
				TOMATOsEngine::PlayAudio(shutdownSoundHandle);
			}
			if (!isShutdown) {
				const uint32_t kArrowAnimation = 25;
				if (!isSwitchViewMode) {
					arrowAnimation++;
					if (arrowAnimation >= kArrowAnimation) {
						if (flag_) {
							arrowColor = 0xFFFFFFFF;
						}
						else {
							arrowColor = 0xFFFFFF00;
						}
						flag_ ^= true;
						arrowAnimation = 0;
					}
					if (TOMATOsEngine::IsKeyTrigger(DIK_S) ||
						TOMATOsEngine::IsKeyTrigger(DIK_DOWN) ||
						((pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) ||
						(pad.Gamepad.sThumbLY < -5000 &&
							prepad.Gamepad.sThumbLY <= 4000.0f&&
							prepad.Gamepad.sThumbLY >= -4000.0f)) {
						if (arrowSetPosition >= 2) {
							arrowSetPosition = 0;
						}
						else {
							arrowSetPosition++;
						}
						TOMATOsEngine::PlayAudio(pickHandle);
						arrowColor = 0xFFFFFFFF;
					}
					if (TOMATOsEngine::IsKeyTrigger(DIK_W) ||
						TOMATOsEngine::IsKeyTrigger(DIK_UP) ||
						((pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) ||
						(pad.Gamepad.sThumbLY > +5000 &&
							prepad.Gamepad.sThumbLY <= 4000.0f &&
							prepad.Gamepad.sThumbLY >= -4000.0f)) {
						if (arrowSetPosition <= 0) {
							arrowSetPosition = 2;
						}
						else {
							arrowSetPosition--;
						}
						TOMATOsEngine::PlayAudio(pickHandle);
						arrowColor = 0xFFFFFFFF;
					}
					if (arrowSetPosition == 0) {
						arrowPosition.y = startTextPosition.y;
					}
					else if (arrowSetPosition == 1) {
						arrowPosition.y = operationTextPosition.y;
					}
					else {
						arrowPosition.y = endTextPosition.y;
					}
					if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
						((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
							!(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
						TOMATOsEngine::PlayAudio(pickHandle);
						if (arrowSetPosition == 0) {
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
						}
						else if (arrowSetPosition == 1) {
							TOMATOsEngine::SwitchViewMode();
							isSwitchViewMode = true;
						}
						else {
							isShutdown = true;
							TOMATOsEngine::PlayAudio(shutdownSoundHandle);
						}
						// スペース押した音
						//auto pushSpacePlayHandle = TOMATOsEngine::PlayAudio(pushSpaceSoundHandle);
						//TOMATOsEngine::SetVolume(pushSpacePlayHandle, 0.1f);
					}


				}
				else if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
					(pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
					!(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
					TOMATOsEngine::PlayAudio(pickHandle);
					TOMATOsEngine::SwitchViewMode();
					isSwitchViewMode = false;
				}
			}
			else {
				animationTime += 1.0f;
				float t = animationTime / kMaxAnimationTime;
				shutdownSize = shutdownSizeAnimation.GetInterpolatedValue(t);
				shutdownSideSize = shutdownSideSizeAnimation.GetInterpolatedValue(t);

				if (animationTime >= kMaxAnimationTime) {
					TOMATOsEngine::RequestQuit();
				}
			}


			TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, titleHandle, 0xFFFFFFFF);

			TOMATOsEngine::DrawSpriteRectAngle(spaceorBPosition, spaceorBSize, { 0.5f,0.5f }, 0.0f, {}, { 150.0f, 32.0f }, spaceorBTextureHandle, 0xFFFFFFFF);
			// スタート
			TOMATOsEngine::DrawSpriteRectAngle(startTextPosition, textSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, startTextureHandle, 0xFFFFFFFF);
			// 操作
			TOMATOsEngine::DrawSpriteRectAngle(operationTextPosition, textSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, operationTextureHandle, 0xFFFFFFFF);
			// 終わり
			TOMATOsEngine::DrawSpriteRectAngle(endTextPosition, textSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, endTextureHandle, 0xFFFFFFFF);
			// 矢印
			TOMATOsEngine::DrawSpriteRectAngle(arrowPosition, arrowSize, { 0.5f,0.5f }, 0.0f, {}, { 32.0f, 32.0f }, arrowTextureHandle, arrowColor);
			if (isShutdown) {
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, { 1280.0f,1280.0f }, { 0.5f,0.5f }, 0.0f, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0x000000FF);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSize, { 0.5f,0.5f }, 0.0f, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFFFF);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSideSize, { 0.5f,0.5f }, 45.0f * Math::ToRadian, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFF05);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSideSize, { 0.5f,0.5f }, 135.0f * Math::ToRadian, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFF05);
			}

			break;
		}

		case inGame:
		{
			if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
				TOMATOsEngine::RequestQuit();
			}
			// 音
			if (field.GetIsVanish() == true) {
				gameScene = gameClear;
			}
			field.Update();
			backGround.Update();
			player.Update();

			// BGMが流れていたらピッチを上げる
			if (ingamePlayHandle != INVALID_PLAY_HANDLE) {
				static float pitch = 1.0f;
				if (field.IsDangerousHeight()) {
					pitch = Math::Lerp(0.1f, pitch, 1.2f);
				}
				else {
					pitch = Math::Lerp(0.1f, pitch, 1.0f);
				}
				TOMATOsEngine::SetPitch(ingamePlayHandle, pitch);
			}

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
					ingamePlayHandle = INVALID_PLAY_HANDLE;
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
					TOMATOsEngine::SetVolume(clearPlayHandle, 0.8f);
					clearToTitle = true;
				}
			}
			if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
				((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
					!(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {

				arrowPosition = { startTextPosition.x - textSize.x * 0.5f - 30.0f,startTextPosition.y };
				arrowSetPosition = 0;
				arrowAnimation = 0;

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
				TOMATOsEngine::SetVolume(titlePlayHandle, 0.2f);
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

	}

	TOMATOsEngine::Shutdown();

	return 0;
}