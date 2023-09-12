#include "FeverManager.h"

#include "TOMATOsEngine.h"

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
}
