#include "FeverManager.h"

#include "TOMATOsEngine.h"

FeverManager* FeverManager::GetInstance() {
	static FeverManager instance;
	return &instance;
}

void FeverManager::Initialize() {
	blockCount_ = 0;
	feverCount_ = 0;
}

void FeverManager::Update() {
	if (isFever_) {
		feverCount_++;
		if (feverCount_ >= kFeverMax_) {
			// リセット
			feverCount_ = 0;
			// フィーバーオン
			isFever_ = false;
		}
	}
	else if (blockCount_ >= kComboMax_) {
		// リセット
		blockCount_ = 0;
		// フィーバーオン
		isFever_ = true;
	}

}

void FeverManager::Draw() {
}
