#include "LevelManager.h"
#include "Field.h"
#include "GameTime.h"
#include "FeverManager.h"

void LevelManager::Initialize() {
	numLever_ = kNumLeversMax - 1;
	interval_ = field_->GetGrowInterval();
	growingBlocks_ = field_->GetNumGrowingBlocks();
}

void LevelManager::Update() {
	if (GameTime::GetInstance()->time_ 
		<= GameTime::GetInstance()->kMaxTime * (float(numLever_) / float(kNumLeversMax))) {
		// 難易度を何倍するか
		const float kLevel = 1.2f;
		// レベル倍率をかける
		interval_ = uint32_t(float(interval_) - (float(interval_) * (kLevel - 1.0f)));
		// インターバルタイムをセット
		field_->SetGrowInterval(interval_);
		numLever_--;
	}
	// フィーバー中
	else if (FeverManager::GetInstance()->GetIsFever()) {
		const uint32_t kInterval = 30;
		const uint32_t kGrowingBlocks = 9;
		// インターバルタイムをセット
		field_->SetGrowInterval(kInterval);
		// 出てくるブロック数
		field_->SetNumGrowingBlocks(kGrowingBlocks);
	}
	else {
		// インターバルタイムをセット
		field_->SetGrowInterval(interval_);
		// 出てくるブロック数
		field_->SetNumGrowingBlocks(growingBlocks_);
	}
}