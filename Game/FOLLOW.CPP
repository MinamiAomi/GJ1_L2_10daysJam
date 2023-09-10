#include "Particle/Follow.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Follow::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	textureHandle_.at(static_cast<uint32_t>(Texture::kWhite1x1)) = TOMATOsEngine::LoadTexture("Resources/Particle/white_particle.png");
	textureHandle_.at(static_cast<uint32_t>(Texture::kStar)) = TOMATOsEngine::LoadTexture("Resources/Particle/star.png");
	textureHandle_.at(static_cast<uint32_t>(Texture::kPlayer)) = TOMATOsEngine::LoadTexture("Resources/player.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
		particle->isAlive_ = false;
	}
	appearCoolTime = 0;
}

void Follow::Create(const Vector2 emitter, Vector4 color, uint32_t textureHandle, uint32_t MaxParticle) {
	appearCoolTime--;
	Random::RandomNumberGenerator rnd{};
	emitter_ = emitter;
	const Vector2 kSize = {30.0f,60.0f};
	const uint32_t kDeath_Time = 15;
	const uint32_t count_Max = MaxParticle;
	uint32_t count = 0;
	if (appearCoolTime < 0) {
		for (auto& particle : particles_) {
			if (count < count_Max && !particle->isAlive_) {
				appearCoolTime = 3;
				// 座標
				particle->position_ = emitter_;
				// 色
				particle->color_ = color;
				// サイズ
				particle->size_Origin_ = kSize;
				particle->size_ = particle->size_Origin_;
				// テクスチャ
				particle->textureHandle_ = textureHandle_.at(textureHandle);
				// 寿命
				particle->time_ = kDeath_Time;
				particle->count_ = 0;

				particle->isAlive_ = true;

				count++;
			}
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
					Math::Lerp(t,0.1f,0.0f));

				// サイズ
				/*float size = Math::Lerp(t, particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };*/
			}
			particle->count_++;
		}
	}
}

void Follow::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			Vector2 rectMinPos = particle->position_ - particle->size_ * 0.5f;
			Vector2 rectMaxPos = particle->position_ + particle->size_ * 0.5f;
			TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, {}, Vector2(30.0f, 60.0f), particle->textureHandle_, Color(particle->color_));
		}
	}
}
