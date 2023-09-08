#include "BackGround.h"

#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

BackGround::BackGround() {
	white_particle_TextureHandle_ = TOMATOsEngine::LoadTexture("Resources/BackGround/white_particle.png");
	// 粒粒生成
	for (auto& grain : grains_) {
		grain = std::make_unique<Grain>();
		grain->position_ = { 0.0f,0.0f };
		grain->size_ = { 0.0f,0.0f };
		grain->color_ = { 0.0f,0.0f,0.0f,0.0f };
		grain->death_Time_ = 0;
		grain->isAlive_ = false;
	}
	grain_TextureHandle_ = white_particle_TextureHandle_;
	Initialize();
}

BackGround::~BackGround() {}

void BackGround::Initialize() {
	GrainInitialize();
}

void BackGround::Update() {
	// 粒粒
	GrainUpdate();
}

void BackGround::Draw() {
	// 粒粒
	GrainDraw();
}

void BackGround::GrainInitialize() {
	count_ = coolTime_;
}

void BackGround::GrainUpdate() {
	count_--;
	if (count_ <= 0) {
		for (auto& grain : grains_) {
			if (!grain->isAlive_) {
				Random::RandomNumberGenerator rnd{};
				const float kSize_Min = 20.0f;
				const float kSize_Max = 30.0f;
				const uint32_t kDeathTime_Min = 10;
				const uint32_t kDeathTime_Max = 20;
				// サイズ
				float size = rnd.NextFloatRange(kSize_Min, kSize_Max);
				grain->size_ = { size,size };
				// ポジション
				float x = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorWidth) - size);
				float y = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorHeight) - size);
				grain->position_ = { x,y };
				// カラー
				grain->color_ = { 1.0f,1.0f,1.0f,1.0f };
				// 時間
				uint32_t death_Time = rnd.NextUIntRange(kDeathTime_Min, kDeathTime_Max);
				grain->death_Time_ = death_Time;
				// フラグ
				grain->isAlive_ = true;
				
				break;
			}
		}
		count_ = coolTime_;
	}
}

void BackGround::GrainDraw() {
	for (auto& grain : grains_) {
		if (grain->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(grain->position_, grain->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), grain_TextureHandle_, 0xFFFFFFFF);
		}
	}
	//TOMATOsEngine::DrawSpriteRect({}, { 1280.0f, 720.0f }, {}, { 1280.0f, 720.0f }, tex, 0xFFFFFFFF);
}
