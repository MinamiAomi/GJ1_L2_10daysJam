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
		Vector4 color_;
		uint32_t death_Time_;
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
private:
	TextureHandle white_particle_TextureHandle_;
	// 粒粒
	static const uint32_t kGrain_Max = 50;
	std::array<std::unique_ptr<Grain>, kGrain_Max> grains_;
	TextureHandle grain_TextureHandle_;
	const uint32_t coolTime_ = 30;
	uint32_t count_;

};

