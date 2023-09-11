#include "LevelManager.h"
#include "Field.h"
#include "GameTime.h"

void LevelManager::Initialize() {
	intervalCount_ = kInterval_ - 1;
}

void LevelManager::Update() {
	if (GameTime::GetInstance()->time_ 
		<= GameTime::GetInstance()->kMaxTime * (float(intervalCount_) / float(kInterval_))) {
		// 難易度を何倍するか
		const float kLevel = 1.2f;
		// 現在のインターバルタイムを取得
		uint32_t interval = field_->GetGrowInterval();
		// レベル倍率をかける
		interval = uint32_t(float(interval) - (float(interval) * (kLevel - 1.0f)));
		// インターバルタイムをセット
		field_->SetGrowInterval(interval);
		intervalCount_--;
	}
}