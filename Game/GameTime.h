#pragma once
#include <cstdint>
#include "Math/MathUtils.h"
#include "TextureHandle.h"
class GameTime
{
public:
	static GameTime* GetInstance();

	static const int32_t kMaxTime = 6;
	static const int32_t kHurryStartTime = 20;
	int32_t time_ = kMaxTime;

	void Initialize();
	void Update();
	void Draw();

	void RaiseTheBGMPitch();
	void LowerTheBGMPitch();
	void StopBGM();
	bool IsHurryTime() const;

	void SetTime(uint32_t time) {
		time_ = time;
	}
	bool GetIsFinish() {
		return isFinish_;
	}
private:
	GameTime();
	~GameTime() = default;
	GameTime(const GameTime&) = delete;
	const GameTime& operator=(const GameTime&) = delete;
	
	void ResetTime() {
		time_ = kMaxTime;
	}

	// テクスチャハンドル
	TextureHandle textureHandle_;
	uint32_t digit_;
	Vector2 size_;
	Vector2 pos_;
	int32_t frameTime_;
	
	size_t bgmSoundHandle_;
	size_t bgmPlayHandle_;

	bool isFinish_;
};

