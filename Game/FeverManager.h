#pragma once

#include <cstdint>

class FeverManager {
public:
	static FeverManager* GetInstance();

	void Initialize();
	void Update();
	void Draw();

	void IncreaseGaugePoints(uint32_t points) { gaugePoints_ += points; }
	void IncreaseBlock(uint32_t blockCount) { gaugePoints_ += blockCount; }
	void IncreaseCombo(uint32_t comboCount) { gaugePoints_ += comboCount * kComboPoints_; }

	bool GetIsFever() {	return isFever_;}

private:
	// ゲージの最大値
	static const uint32_t kFeverGaugeMax_ = 80;
	// コンボ時の上昇量
	static const uint32_t kComboPoints_ = 8;
	// フィーバーの時間
	static const uint32_t kFeverMaxFrame_ = 360;
	// 周期のリセット時間
	static const uint32_t kFrameCycle = 60;
	
	// ゲージの量
	uint32_t gaugePoints_;
	// 周期(いろいろ使う)
	uint32_t frame_;
	bool isFever_;

};

