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
		TextureHandle textureHandle_;
		bool isAlive_;
	};
public:
	enum class Texture {
		kWhite1x1,
		kCount,
	};

public:
	void Initialize();
	void Create(const Vector2 emitter, Vector4 color, uint32_t textureHandle, bool right = true);
	void Update();
	void Draw();
private:
	Vector2 emitter_;
	static const size_t kTextureMax = static_cast<size_t>(Texture::kCount);
	std::array<TextureHandle, kTextureMax > textureHandle_;
	std::array<std::unique_ptr<Particle>, 80> particles_;
	float angle_X_;
	float angle_Y_;
};

