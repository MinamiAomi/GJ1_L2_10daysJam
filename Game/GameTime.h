#pragma once
#include <stdint.h>
#include "Math/MathUtils.h"
#include "TextureHandle.h"
class GameTime
{
public:
	static GameTime* GetInstance();
	static const int maxTime = 60;
	static int32_t time_;
	void Initialize();
	void Update();
	void Draw();
	void SetTime(uint32_t time) {
		time_ = time;
	}
private:
	// テクスチャハンドル
	TextureHandle textureHandle_;
	uint32_t digit_;
	Vector2 size_;
	Vector2 pos_;
	int32_t frameTime_;
	GameTime();
	~GameTime() = default;
	GameTime(const GameTime&) = delete;
	const GameTime& operator=(const GameTime&) = delete;
	void ResetTime() {
		time_ = maxTime;
	}

};

