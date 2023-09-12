#pragma once

#include <cstdint>

class FeverManager {
public:
	static FeverManager* GetInstance();

	void Initialize();
	void Update();
	void Draw();

	void SetBlockCount(uint32_t block) { blockCount_ += block; }
	void SetComboCount(uint32_t combo) { blockCount_ += kTripleConbo_; }
	bool GetIsFever() {	return isFever_;}
private:
	const uint32_t kComboMax_ = 80;
	const uint32_t kTripleConbo_ = 8;
	const uint32_t kFeverMax_ = 360;
	
	uint32_t blockCount_;
	uint32_t feverCount_;
	bool isFever_;

};

