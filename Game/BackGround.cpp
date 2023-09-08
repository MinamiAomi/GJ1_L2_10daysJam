#include "BackGround.h"

#include "Math/Color.h"
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
	// 円
	for (auto& circle : circles_) {
		circle = std::make_unique<Circle>();
		circle->position_ = { 0.0f,0.0f };
		circle->velocity_ = { 0.0f,0.0f };
		circle->size_ = { 0.0f,0.0f };
		circle->color_ = { 0.0f,0.0f,0.0f,0.0f };
		circle->death_Time_ = 0;
		circle->isAlive_ = false;
	}
	circle_TextureHandle_ = white_particle_TextureHandle_;
	Initialize();
}

BackGround::~BackGround() {}

void BackGround::Initialize() {
	// 粒粒
	GrainInitialize();
	// 円
	CircleInitialize();
}

void BackGround::Update() {

	// 粒粒
	GrainUpdate();
	// 円
	CircleUpdate();
}

void BackGround::Draw() {

	// 粒粒
	GrainDraw();
	// 円
	CircleDraw();
}

void BackGround::GrainInitialize() {
	grain_Count_ = grain_CoolTime_;
}

void BackGround::GrainUpdate() {
	grain_Count_--;
	if (grain_Count_ <= 0) {
		for (auto& grain : grains_) {
			if (!grain->isAlive_) {
				Random::RandomNumberGenerator rnd{};
				const float kSize_Min = 5.0f;
				const float kSize_Max = 10.0f;
				const uint32_t kDeathTime_Min = 360;
				const uint32_t kDeathTime_Max = 600;
				// サイズ
				float size = rnd.NextFloatRange(kSize_Min, kSize_Max);
				grain->size_Original_ = { size,size };
				grain->size_ = grain->size_Original_;
				// ポジション
				float x = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorWidth) - size);
				float y = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorHeight) - size);
				grain->position_ = { x,y };
				// カラー
				grain->color_ = { 1.0f,1.0f,1.0f,0.0f };
				// 時間
				uint32_t death_Time = rnd.NextUIntRange(kDeathTime_Min, kDeathTime_Max);
				grain->death_Time_ = death_Time;
				grain->death_Count_ = 0;
				// フラグ
				grain->isAlive_ = true;

				break;
			}
		}
		grain_Count_ = grain_CoolTime_;
	}
	for (auto& grain : grains_) {
		if (grain->isAlive_) {
			grain->death_Count_++;
			if (grain->death_Count_ < grain->death_Time_ * 0.5f) {
				grain->size_.x = Math::Lerp(
					std::clamp(
						static_cast<float>(grain->death_Count_)
						/ (static_cast<float>(grain->death_Time_) * 0.5f), 0.0f, 1.0f),
					0.0f, grain->size_Original_.x);
				grain->size_.y = grain->size_.x;
				grain->color_.w = Math::Lerp(
					std::clamp(
						static_cast<float>(grain->death_Count_)
						/ (static_cast<float>(grain->death_Time_) * 0.5f), 0.0f, 1.0f),
					0.0f, 0.8f);
			}
			else {
				grain->size_.x = Math::Lerp(
					std::clamp(
						(static_cast<float>(grain->death_Count_) - (static_cast<float>(grain->death_Time_) * 0.5f)) /
						(static_cast<float>(grain->death_Time_) * 0.5f),
						0.0f, 1.0f),
					grain->size_Original_.x, 0.0f);
				grain->size_.y = grain->size_.x;

				grain->color_.w = Math::Lerp(
					std::clamp(
						(static_cast<float>(grain->death_Count_) - (static_cast<float>(grain->death_Time_) * 0.5f)) /
						(static_cast<float>(grain->death_Time_) * 0.5f),
						0.0f, 1.0f),
					0.8f, 0.0f);
			}
			if (grain->death_Count_ >= grain->death_Time_) {
				grain->isAlive_ = false;
			}
		}

	}
}

void BackGround::GrainDraw() {
	for (auto& grain : grains_) {
		if (grain->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(grain->position_, grain->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), grain_TextureHandle_, Color(grain->color_));
		}
	}
}

void BackGround::CircleInitialize() {
	circle_Emitter_ = {0.0f,0.0f };
	angle_ = 0.0f;
	circle_Count_ = circle_CoolTime_;
}

void BackGround::CircleUpdate() {
	const Vector2  distance = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };
	const float kRadius = 10.0f;
	const float kAddAngle = 60.0f / 360.0f;
	angle_ += kAddAngle;
	circle_Emitter_.x = distance.x + kRadius * std::cos(angle_);
	circle_Emitter_.y = distance.y + kRadius * std::sin(angle_);
	if (angle_ >= 360.0f) {
		angle_ = 0.0f;
	}
	circle_Count_--;
	if (circle_Count_ <= 0) {
		for (auto& circle : circles_) {
			if (!circle->isAlive_) {
				const uint32_t death_Time = 30;
				// ポジション
				circle->position_ = circle_Emitter_;
				// 速度
				circle->velocity_ = circle_Emitter_ - distance;
				circle->velocity_.Normalize();
				// カラー
				circle->color_ = { 0.0f,0.5f,0.5f,1.0f };
				// 時間
				circle->death_Time_ = death_Time;
				circle->death_Count_ = 0;
				// フラグ
				circle->isAlive_ = true;

				break;
			}
		}
		circle_Count_ = circle_CoolTime_;
	}
	for (auto& circle : circles_) {
		if (circle->isAlive_) {
			const float size = 10.0f;
			const float speed = 5.0f;
			circle->death_Count_++;
			// 速度
			circle->position_ += circle->velocity_ * speed;
			// サイズ
			circle->size_.x = Math::Lerp(
				std::clamp(
					(static_cast<float>(circle->death_Count_)) /
					(static_cast<float>(circle->death_Time_)),
					0.0f, 1.0f),
				5.0f, size);
			circle->size_.y = circle->size_.x;

			circle->color_.w = Math::Lerp(
				std::clamp(
					(static_cast<float>(circle->death_Count_)) /
					(static_cast<float>(circle->death_Time_)),
					0.0f, 1.0f),
				1.0f, 0.0f);
		}
		if (circle->death_Count_ >= circle->death_Time_) {
			circle->isAlive_ = false;
		}
	}

}

void BackGround::CircleDraw() {
	for (auto& circle : circles_) {
		if (circle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(circle->position_, circle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), grain_TextureHandle_, Color(circle->color_));
		}
	}
}
