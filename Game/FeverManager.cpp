#include "FeverManager.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"

FeverManager* FeverManager::GetInstance() {
    static FeverManager instance;
    return &instance;
}

void FeverManager::Initialize() {
    gaugePoints_ = 0;
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
    float gaugeRatio = float(gaugePoints_) / kFeverGaugeMax_;
    // 波の基準の高さ
    float waveBaseHeight = kFeverStartWaveHeight * gaugeRatio;
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
    const float waveWidth = 10.0f;
    for (uint32_t i = 0; i < kNumWaveDivisions; ++i) {
        float left = xSlice * i;
        float right = left + xSlice;
        float top0 = heights[i];
        float top1 = heights[i + 1];
        float bottom0 = top0 - waveWidth;
        float bottom1 = top1 - waveWidth;
        uint32_t color0 = colors[i];
        uint32_t color1 = colors[i + 1];

        TOMATOsEngine::DrawTriangle({ left, bottom0 }, color0, { left, top0 }, color0, { right, top1 }, color1);
        TOMATOsEngine::DrawTriangle({ left, bottom0 }, color0, { right, top1 }, color1, { right, bottom1 }, color1);
    }
}
