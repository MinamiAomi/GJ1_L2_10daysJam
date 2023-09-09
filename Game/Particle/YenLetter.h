#pragma once
#include <cstdint>
#include <memory>
#include <array>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class YenLetter {
private:
	struct Particle {
		Vector2 position_;
		Vector4 color_;
		Vector2 size_;
		Vector2 size_Origin_;
		uint32_t time_;
		uint32_t count_;
		bool isAlive_;
	};

public:
	void Initialize();
	void Create(const Vector2 emitter, bool right = true);
	void Update();
	void Draw();
private:
	Vector2 emitter_;
	TextureHandle textureHandle_;
	std::array<std::unique_ptr<Particle>, 80> particles_;
	float angle_X_;
	float angle_Y_;
};

