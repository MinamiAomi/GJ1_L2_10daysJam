#include "VerticalLine.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"


void VerticalLine::Initialize() {
	textureHandle_.at(static_cast<uint32_t>(Texture::kHorizonLine)) = TOMATOsEngine::LoadTexture("Resources/Particle/horizonLine.png");
	textureHandle_.at(static_cast<uint32_t>(Texture::kBeam)) = TOMATOsEngine::LoadTexture("Resources/Particle/beam.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void VerticalLine::Create(const Vector2 emitter, Vector4 color, uint32_t textureHandle,Vector2 size) {
	Random::RandomNumberGenerator rnd{};
	const float kSpeed =80.0f;
	const uint32_t count_Max = 1;
	uint32_t count = 0;
	emitter_ = emitter;
	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = color;
			// 速度
			particle->velocity_.y = kSpeed;
			// サイズ
			particle->size_ = size;
			// テクスチャ
			particle->textureHandle_ = textureHandle_.at(textureHandle);

			particle->isAlive_ = true;

			count++;
		}
	}
}

void VerticalLine::Update() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			if (particle->position_.y >=float(TOMATOsEngine::kMonitorHeight) + particle->size_.y) {
				particle->isAlive_ = false;
			}
			else {
				// 移動
				particle->position_ += particle->velocity_;
			}
		}
	}
}

void VerticalLine::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 64.0f), particle->textureHandle_, Color(particle->color_));
		}
	}
}
