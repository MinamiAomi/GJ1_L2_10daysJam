#pragma once
#include <stdint.h>
#include "Math/MathUtils.h"
#include "TextureHandle.h"
class GameTime
{
public:
	static GameTime* GetInstance();
	const int maxTime = 1;
	int32_t time_ = maxTime;
	void Initialize();
	void Update();
	void Draw();

	void SetTime(uint32_t time) {
		time_ = time;
	}
	bool GetIsFinish() {
		return isFinish;
	}
private:
	// テクスチャハンドル
	TextureHandle textureHandle_;
	uint32_t digit_;
	Vector2 size_;
	Vector2 pos_;
	int32_t frameTime_;
	bool isFinish;
	GameTime();
	~GameTime() = default;
	GameTime(const GameTime&) = delete;
	const GameTime& operator=(const GameTime&) = delete;
	void ResetTime() {
		time_ = maxTime;
	}

};

