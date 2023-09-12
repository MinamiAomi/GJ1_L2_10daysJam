#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Field;
class LevelManager {
public:
	struct Effect {
		Vector2 position_;
		Vector2 velocity_;
		Vector2 size_;
		float colorH_;
		bool isAlive_;
	};
public:
	void Initialize();
	void Update();
	void Draw();

	void GetFild(Field* field) { field_ = field; }
private:
	void LevelUpdate();
	void EffectUpdate();
	void CreateEffect();

	Field* field_;
	
	static const uint32_t kEffectMax = 10;
	const uint32_t kNumLeversMax = 10;
	uint32_t numLever_;
	uint32_t interval_;
	uint32_t growingBlocks_;
	uint32_t time_;

	std::array<std::unique_ptr<Effect> , kEffectMax > effects_;

	TextureHandle textureHandle_;
};

