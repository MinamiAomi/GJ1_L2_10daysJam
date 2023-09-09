#include "Particle/Pop.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Pop::Initialize() {
	emitter_ = {0.0f, 0.0f};
	textureHandle_.at(static_cast<uint32_t>(Texture::kWhite1x1)) = TOMATOsEngine::LoadTexture("Resources/Particle/white1x1.png");
	textureHandle_.at(static_cast<uint32_t>(Texture::kBlock)) = TOMATOsEngine::LoadTexture("Resources/block.png");
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Pop::Create(const Vector2 emitter, Vector4 color,uint32_t textureHandle, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	const uint32_t deathtime_Min = 5;
	const uint32_t deathtime_Max = 10;
	const float size_Min = 10.0f;
	const float size_Max = 15.0f;
	const uint32_t count_Max = MaxParticle;
	uint32_t count = 0;
	emitter_ = emitter;
	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = color;
			// 速度
			float velocity_X = rnd.NextFloatRange(-100.0f, 100.0f);
			float velocity_Y = rnd.NextFloatRange(-100.0f, 100.0f);
			Vector2 velocity = {velocity_X, velocity_Y};
			velocity.Normalize(); 
			float speed = rnd.NextFloatRange(size_Min, size_Max);
			particle->velocity_ = velocity * speed;
			// 加速度
			particle->acceleration_ = {0.0f, 0.0f};
			// サイズ
			float size = rnd.NextFloatRange(size_Min, size_Max);
			particle->size_Origin_ = {size, size};
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

void Pop::Update() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			particle->count_++;
			if (particle->count_ >= particle->time_) {
				particle->isAlive_ = false;
			} else {
				// 移動
				particle->position_ += particle->velocity_;
			}
		}
	}
}

void Pop::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(particle->position_, particle->size_, Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(32.0f, 32.0f), particle->textureHandle_, Color(particle->color_));
		}
	}
}
