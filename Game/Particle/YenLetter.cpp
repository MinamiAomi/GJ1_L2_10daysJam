#include "Particle/YenLetter.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"
void YenLetter::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	textureHandle_ = TOMATOsEngine::LoadTexture("Resources/Particle/white_particle.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
		particle->isAlive_ = false;
	}
	angle_X_ = 0.0f;
	angle_Y_ = 0.0f;
}

void YenLetter::Create(const Vector2 emitter, bool right) {
	Random::RandomNumberGenerator rnd{};

	const float radius = 32.0f;
	const float addAngle = 30.0f * Math::ToRadian;
	if (right) {
		angle_X_ += addAngle;
		if (angle_X_ >= 360.0f * Math::ToRadian) {
			angle_X_ = 0.0f;
		}
		emitter_.x = emitter.x + radius * std::cos(angle_X_);
		emitter_.y = emitter.y /*+ radius * std::sin(angle_)*/;
	}
	else {
		angle_Y_ -= addAngle;
		if (angle_Y_ <= -360.0f * Math::ToRadian) {
			angle_Y_ = 0.0f;
		}
		emitter_.x = emitter.x + radius * std::sin(angle_Y_);
		emitter_.y = emitter.y /*+ radius * std::sin(angle_Y_)*/;
	}
	
	const float kSize = 5.0f;
	const uint32_t kDeath_Time = 30;
	const uint32_t count_Max = 1;
	uint32_t count = 0;

	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
			// サイズ
			particle->size_Origin_ = { kSize,kSize };
			particle->size_ = particle->size_Origin_;
			// 寿命
			particle->time_ = kDeath_Time;
			particle->count_ = 0;

			particle->isAlive_ = true;

			count++;
		}
	}
}

void YenLetter::Update() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			if (particle->count_ >= particle->time_) {
				particle->isAlive_ = false;
			}
			else {
				float t = std::clamp(
					static_cast<float>(particle->count_) /
					static_cast<float>(particle->time_),
					0.0f, 1.0f);
				// 色
				particle->color_ = Vector4(
					1.0f, 1.0f, 1.0f,
					Math::Lerp(t, 1.0f, 0.0f));

				// サイズ
				float size = Math::Lerp(t, particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };
			}
			particle->count_++;
		}
	}
}

void YenLetter::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), textureHandle_, Color(particle->color_));
		}
	}
}
