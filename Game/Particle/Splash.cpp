#include "Particle/Splash.h"

#include <iomanip>
#include <sstream>

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Splash::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	textureHandle_.at(static_cast<uint32_t>(Texture::kWhite1x1)) = TOMATOsEngine::LoadTexture("Resources/Particle/white1x1.png");
	textureHandle_.at(static_cast<uint32_t>(Texture::kBlock)) = TOMATOsEngine::LoadTexture("Resources/block.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Splash::Create(const Vector2 emitter, Vector4 color,uint32_t textureHandle, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	emitter_ = emitter;
	const uint32_t deathtime_Min = 8;
	const uint32_t deathtime_Max = 16;
	const float speed_Min = 5.0f;
	const float speed_Max = 10.0f;
	const float size_Min = 5.0f;
	const float size_Max = 10.0f;
	const uint32_t count_Max = MaxParticle;
	uint32_t count = 0;

	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = color;
			// 速度
			Vector2 move{};
			move.x = std::cos(rnd.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
			move.y = std::sin(rnd.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
			move.Normalize();
			// すくりん変換用逆
			float speed = rnd.NextFloatRange(speed_Min, speed_Max);
			particle->velocity_ = move * speed;
			// 加速度
			particle->acceleration_ = { 0.0f, 0.0f };
			// サイズ
			float size = rnd.NextFloatRange(size_Min, size_Max);
			particle->size_Origin_ = { size, size };
			particle->size_ = particle->size_Origin_;
			// テクスチャ
			particle->textureHandle_ = textureHandle_.at(textureHandle);
			// 寿命
			particle->time_ = rnd.NextUIntRange(deathtime_Min, deathtime_Max);
			particle->count_ = 0;

			particle->isAlive_ = true;

			count++;
		}
	}
}

void Splash::Update() {
	const float kGravity = -1.0f;
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			particle->count_++;
			if (particle->count_ >= particle->time_) {
				particle->isAlive_ = false;
			}
			else {
				float t = std::clamp(
					static_cast<float>(particle->count_) /
					static_cast<float>(particle->time_),
					0.0f, 1.0f);
				// 移動
				particle->velocity_.y += kGravity;
				particle->acceleration_ += particle->velocity_;
				particle->position_ += particle->acceleration_;
				particle->acceleration_ = Vector2(0.0f, 0.0f);
				// 色
				particle->color_ = Vector4(
					1.0f, 1.0f, 1.0f,
					Math::Lerp(t, 1.0f, 0.0f));

				// サイズ
				float size = Math::Lerp(t, particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };
			}
		}
	}
}

void Splash::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f , {}, Vector2(32.0f, 32.0f), particle->textureHandle_,  Color(particle->color_));
		}
	}
}
