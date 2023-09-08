#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class BackGround {
private:
	// 粒粒
	struct Grain {
		Vector2 position_;
		Vector2 size_;
		Vector2 size_Original_;
		Vector4 color_;
		uint32_t death_Time_;
		uint32_t death_Count_;
		bool isAlive_;
	};
	// 円
	struct Circle {
		Vector2 position_;
		Vector2 velocity_;
		Vector2 size_;
		Vector4 color_;
		uint32_t death_Time_;
		uint32_t death_Count_;
		bool isAlive_;
	};
public:
	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void Draw();
private:
	// 粒粒
	void GrainInitialize();
	void GrainUpdate();
	void GrainDraw();
	// 円
	void CircleInitialize();
	void CircleUpdate();
	void CircleDraw();
private:
	TextureHandle white_particle_TextureHandle_;
	// 粒粒
	static const uint32_t kGrain_Max = 16;
	std::array<std::unique_ptr<Grain>, kGrain_Max> grains_;
	TextureHandle grain_TextureHandle_;
	const uint32_t grain_CoolTime_ = 120;
	uint32_t grain_Count_;

	// くるくる
	Vector2 circle_Emitter_;
	float angle_;
	static const uint32_t kCircle_Max = 50;
	std::array<std::unique_ptr<Circle>, kCircle_Max> circles_;
	TextureHandle circle_TextureHandle_;
	const uint32_t circle_CoolTime_ = 2;
	uint32_t circle_Count_;
};

