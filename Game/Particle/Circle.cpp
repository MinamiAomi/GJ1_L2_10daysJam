#include "Particle/Circle.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"


void Circle::Initialize() {
	emitter_ = {0.0f, 0.0f};
	textureHandle_ = TOMATOsEngine::LoadTexture("Resources/Particle/circle.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Circle::Create(const Vector2 emitter, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	emitter_ = emitter;
	const uint32_t deathtime_Min = 15;
	const uint32_t deathtime_Max = 30;
	const float size_Min = 10.0f;
	const float size_Max = 50.0f;
	const uint32_t count_Max = MaxParticle;
	uint32_t count = 0;

	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			// 速度
			particle->velocity_ = {0.0f,0.0f};
			// 加速度
			particle->acceleration_ = {0.0f, 0.0f};
			// サイズ
			float size = rnd.NextFloatRange(size_Min, size_Max);
			particle->size_Origin_ = {size, size};
			particle->size_ = {0.0f,0.0f};
			// 寿命
			particle->time_ = rnd.NextUIntRange(deathtime_Min, deathtime_Max);
			particle->count_ = 0;

			particle->isAlive_ = true;

			count++;
		}
	}
}

void Circle::Update() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			particle->count_++;
			if (particle->count_ >= particle->time_) {
				particle->isAlive_ = false;
			} else {
				// 色
				particle->color_ = Vector4(
				    1.0f, 1.0f, 1.0f,
					Math::Lerp(
				        1.0f, 0.0f,
						std::clamp(
				            static_cast<float>(particle->count_) /
				                static_cast<float>(particle->time_),
				            0.0f, 1.0f)));

				// サイズ
				float size = Math::Lerp(
				    0.0f, 
					particle->size_Origin_.x,
					std::clamp(
				        static_cast<float>(particle->count_) /
				            static_cast<float>(particle->time_),
				        0.0f, 1.0f));
				particle->size_ = {size, size};
			}
		}
	}
}

void Circle::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), textureHandle_, Color(particle->color_));
		}
	}
}
