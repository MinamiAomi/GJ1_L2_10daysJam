#include "FeverManager.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"

FeverManager* FeverManager::GetInstance() {
	static FeverManager instance;
	return &instance;
}

void FeverManager::Initialize() {
	gaugePoints_ = 0;
	delayGaugePoints_ = 0.0f;
	preGaugePoints_ = gaugePoints_;
	diffirentCount_ = 0;
	isDifferent_ = false;

	textureHandle_ = TOMATOsEngine::LoadTexture("Resources/fever.png");
	texturePosition_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,0.0f };
	float scale = 3.0f;
	textureSize_ = { 76.0f* scale , 32.0f * scale };
}

void FeverManager::Update() {
	++frame_;
	if (frame_ >= kFrameCycle) {
		frame_ = 0;
	}

	if (isFever_) {
		if ((frame_ % uint32_t(kFeverMaxFrame_ / kFeverGaugeMax_)) == 0) {
			--gaugePoints_;
		}
		if (gaugePoints_ <= 0) {
			// リセット
			gaugePoints_ = 0;
			// フィーバーOFF
			isFever_ = false;
		}
	}
	else if (gaugePoints_ >= kFeverGaugeMax_) {
		// リセット
		gaugePoints_ = kFeverGaugeMax_;
		// フィーバーオン
		isFever_ = true;
	}

}

void FeverManager::Draw() {
	const uint32_t kNumWaveDivisions = 32;
	const float kWaveHeight = 10.0f;
	const float kNumWavesInMonitor = 5.0f;
	// フィーバー発動時の波の高さ
	const float kFeverStartWaveHeight = TOMATOsEngine::kMonitorHeight - 40.0f;

	// ゲージの割合
	delayGaugePoints_ = Math::Lerp(0.2f, delayGaugePoints_, float(gaugePoints_));
	float gaugeRatio = delayGaugePoints_ / kFeverGaugeMax_;
	// 波の基準の高さ
	float waveBaseHeight = kFeverStartWaveHeight * gaugeRatio;
	texturePosition_.y = kFeverStartWaveHeight * gaugeRatio;
	float frameT = frame_ / float(kFrameCycle);
	// 角度のオフセット
	float angleOffset = Math::TwoPi * frameT;

	// 一つの波の角度
	float angleSlice = kNumWavesInMonitor * Math::TwoPi / kNumWaveDivisions;
	// 一つのHの増加量
	float colorHSlice = 1.0f / kNumWaveDivisions;
	// 高さ
	float heights[kNumWaveDivisions + 1]{};
	uint32_t colors[kNumWaveDivisions + 1]{};
	for (uint32_t i = 0; i < kNumWaveDivisions + 1; ++i) {
		float angle = i * angleSlice + angleOffset;
		heights[i] = std::sin(angle) * kWaveHeight + waveBaseHeight;
		colors[i] = Color::HSVA(colorHSlice * i + frameT, 1.0f, 1.0f, 0.2f);
		//colors[i] = 0x222222FF;
	}

	const float xSlice = 1.0f / kNumWaveDivisions * TOMATOsEngine::kMonitorWidth;
	float waveWidth = 0.0f;
	const uint32_t kDiffirentTime = 10;
	bool isDraw = true;
	if (!isDifferent_ &&
		gaugePoints_ != preGaugePoints_) {
		isDifferent_ = true;
		diffirentCount_ = 0;
	}
	uint32_t color0 = 0;
	uint32_t color1 = 0;
	for (uint32_t i = 0; i < kNumWaveDivisions; ++i) {
		// フィーバー中
		if (isFever_) {
			color0 = colors[i];
			color1 = colors[i + 1];
			waveWidth = 10.0f;
		} // 前回と値が変わらない
		else if (!isDifferent_) {
			color0 = colors[i];
			color1 = colors[i + 1];
			waveWidth = 10.0f;
		}
		else {
			if (diffirentCount_ % 4 == 0) {
				waveWidth = 0.0f;
				color0 = Color::HSVA(1.0f, 0.0f, 0.3f, 0.3f);
				color1 = Color::HSVA(1.0f, 0.0f, 0.3f, 0.3f);
				isDraw = false;
			}
			else {
				waveWidth = 5.0f;
				color0 = Color::HSVA(1.0f, 0.0f, 0.5f, 0.5f);
				color1 = Color::HSVA(1.0f, 0.0f, 0.5f, 0.5f);
			}
			
		}

		float left = xSlice * i;
		float right = left + xSlice;
		float top0 = heights[i];
		float top1 = heights[i + 1];
		float bottom0 = top0 - waveWidth;
		float bottom1 = top1 - waveWidth;
		if(isDraw){
		TOMATOsEngine::DrawTriangle({ left, bottom0 }, color0, { left, top0 }, color0, { right, top1 }, color1);
		TOMATOsEngine::DrawTriangle({ left, bottom0 }, color0, { right, top1 }, color1, { right, bottom1 }, color1);
		}
	}
	if (diffirentCount_ >= kDiffirentTime) {
		isDifferent_ = false;
	}
	if (isDifferent_) {
		diffirentCount_++;
	}
	preGaugePoints_ = gaugePoints_;
	// テクスチャ
	/*if (isFever_) {
		TOMATOsEngine::DrawSpriteRectAngle(texturePosition_, textureSize_, { 0.5f,0.5f }, 0.0f, {}, { 76.0f,32.0f }, textureHandle_, Color::HSVA(colorHSlice + frameT, 1.0f, 1.0f, 0.4f));
	}*/
}
