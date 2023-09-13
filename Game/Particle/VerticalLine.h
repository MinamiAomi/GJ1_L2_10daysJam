#pragma once

#include <cstdint>
#include <memory>
#include <array>

#include "Math/MathUtils.h"
#include "TextureHandle.h"


class VerticalLine {
private:
	struct Particle {
		Vector2 position_;
		Vector2 velocity_;
		Vector2 size_;
		Vector4 color_;
		float colorH_;
		TextureHandle textureHandle_;
		bool isAlive_;
	};
public:
	enum class Texture {
		kHorizonLine,
		kBeam,
		kCount,
	};

public:
	void Initialize();
	void Create(const Vector2 emitter, Vector4 color, uint32_t textureHandle, Vector2 size);
	void Update();
	void Draw();
private:
	Vector2 emitter_;
	static const size_t kTextureMax = static_cast<size_t>(Texture::kCount);
	std::array<TextureHandle, kTextureMax > textureHandle_;
	std::array<std::unique_ptr<Particle>, 80> particles_;
};

