#include "Particle/Follow.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Follow::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	textureHandle_ = TOMATOsEngine::LoadTexture("Resources/Particle/white_particle.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
		particle->isAlive_ = false;
	}
}

void Follow::Create(const Vector2 emitter, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	emitter_ = emitter;
	const float kSize = 10.0f;
	const uint32_t kDeath_Time = 30;
	const uint32_t count_Max = MaxParticle;
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

void Follow::Update() {
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
					Math::Lerp(t,1.0f,0.0f));

				// サイズ
				float size = Math::Lerp(t, particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };
			}
			particle->count_++;
		}
	}
}

void Follow::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), textureHandle_, Color(particle->color_));
		}
	}
}
