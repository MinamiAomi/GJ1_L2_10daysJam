#include "Field.h"

#include <numeric>

#include "TOMATOsEngine.h"
#include "Particle/ParticleManager.h"
#include "Math/Color.h"

#include "FeverManager.h"
#include "GameTime.h"
#include "Easing.h"

#include "Player.h"

void Field::Initialize() {
	memset(blocks_, BlockType::None, sizeof(blocks_));

	fieldSize_ = { float(kBlockSize * kNumHorizontalBlocks), float(kBlockSize * kNumVerticalBlocks) };

	const uint32_t kHeight = 2;
	for (uint32_t y = 0; y < kHeight; y++) {
		for (uint32_t x = 0; x < kNumHorizontalBlocks; x++) {
			blocks_[x][y] = BlockType::Normal;
		}
	}
	GrowField(5);
	GrowField(5);
	// 次成長するところをセット
	nextBlockIndices_ = GetGrowField(numGrowingBlocks_);
	// 色
	initializeColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	ColorClearBlock();
	growCoolTime_ = 0;

	growAnimationCount_ = 0;
	growAnimationFrame_ = 0;
	growAnimationFrameSize_ = 4;

	breakTime_ = 0;
	downBlockIndex_ = 0;
	isFlash_ = false;

	textureHandles_.at(Texture::kBlock) = TOMATOsEngine::LoadTexture("Resources/block.png");
	textureHandles_.at(Texture::kGrow) = TOMATOsEngine::LoadTexture("Resources/grow.png");
	textureHandles_.at(Texture::kGameOverBlock) = TOMATOsEngine::LoadTexture("Resources/gameOverBlock.png");
	textureHandles_.at(Texture::kGameOver) = TOMATOsEngine::LoadTexture("Resources/gameOver.png");
	textureHandles_.at(Texture::kDeadLine) = TOMATOsEngine::LoadTexture("Resources/deadLine_1.png");

	dangerousBlockTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/dangerousBlock.png");
	deadLinePosition_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f ,float(Field::kBlockSize) * float(kDeathLine_)/* + float(Field::kBlockSize) * 0.5f*/ };
	deadLineSize_ = { 630.0f ,16.0f };
	deadLineAnimationFront_ = 0.0f;
	deadLineAnimationBack_ = deadLineSize_.x;
	deadLineColorH_ = 0.0f;

	heightCount_ = kDeathLine_;
	blockBleakAnimationCount_ = 0;
	gameOverBlocks_.clear();

	gameOverPosition_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,float(TOMATOsEngine::kMonitorHeight) + float(TOMATOsEngine::kMonitorHeight) * 0.5f };
	gameOverPositionStart_ = gameOverPosition_;
	gameOverPositionEnd_ = { gameOverPosition_.x,float(TOMATOsEngine::kMonitorHeight) * 0.5f };
	dropTextCount_ = 0;
	gameOverBlockCount_ = 0;

	isBlockBreaking_ = false;
	isTextDropping_ = false;
	isGameOver_ = false;
	isInGameOver_ = false;
	isVanish_ = false;

	breakedBlockNum_ = 0;
	combedStepNum_ = 0;
	combedHrizonNum_ = 0;
	breakSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/break.wav");
	lineBreakSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/lineBreak.wav");
	scoreTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/score.png");
	numTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/dekisokonai36.png");
	scoreT = 0.0f;

	blockDownHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/blockDown.wav");
	blockDown2Handle_ = TOMATOsEngine::LoadAudio("Resources/Audio/blockDown2.wav");
	scoreSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/score.wav");

	growCoolTime_ = 0;
	growInterval_ = 120;
	numGrowingBlocks_ = 4;
	scoreFrame_ = 0;
}

void Field::Update() {
	GameTime* gameTime = GameTime::GetInstance();

	if (gameTime->GetIsFinish() && isFlash_ == false) {
		breakTime_--;
		ClearBreakBlockHorizon();
		if (breakTime_ > 0) {

		}
		else {
			if (isClearFlash_ == true) {
				isClearFlash_ = false;
				for (uint32_t x = 0; x < kNumVerticalBlocks; x++) {
					for (uint32_t y = 0; y < kNumHorizontalBlocks; y++) {
						if (blocks_[x][y] == BlockType::Frash) {
							blocks_[x][y] = BlockType::None;
						}
					}
				}
			}
		}
	}
	else
		if (!isInGameOver_) {
			ChackBlock();
			++growCoolTime_;
			breakTime_--;
			if (growCoolTime_ >= growInterval_ && isFlash_ == false) {
				// 成長
				SetGrow(nextBlockIndices_, numGrowingBlocks_);
				// 次成長するところをセット
				nextBlockIndices_ = GetGrowField(numGrowingBlocks_);
				growCoolTime_ = 0;
			}


			if (breakTime_ > 0) {

			}
			else {
				if (isFlash_ == true) {
					isFlash_ = false;
					for (uint32_t x = 0; x < kNumVerticalBlocks; x++) {
						for (uint32_t y = 0; y < kNumHorizontalBlocks; y++) {
							if (blocks_[x][y] == BlockType::Frash) {
								blocks_[x][y] = BlockType::None;
							}
						}
					}
					DownBlockHorizon();
				}
			}
			HarryEffect();
		}
		else {
			GameOverUpdate();
		}
}

void Field::Draw() {
	TOMATOsEngine::SetBlendMode(kBlendModeNormal);
	// でっとライン
	const float kDeadLineAnimationCount = 2.0f;
	if (deadLineAnimationFront_ >= deadLineSize_.x) {
		deadLineAnimationFront_ = 0.0f;
		deadLineAnimationBack_ = deadLineSize_.x;
	}
	deadLineAnimationFront_ += kDeadLineAnimationCount;
	deadLineAnimationBack_ += kDeadLineAnimationCount;

	TOMATOsEngine::DrawSpriteRectAngle(deadLinePosition_, deadLineSize_, { 0.5f,0.5f }, 0.0f, { deadLineAnimationFront_ ,16.0f }, deadLineSize_, textureHandles_.at(Texture::kDeadLine), Color::HSVA(deadLineColorH_, 0.8f, 0.8f));
	DrawBlock();
	GameTime* gameTime = GameTime::GetInstance();
	if (!gameTime->GetIsFinish()) {
		DrawGrow();
	}
}

void Field::DrawScore() {
	Vector2 startPos = { (float)TOMATOsEngine::kMonitorWidth + TOMATOsEngine::kMonitorWidth * 0.5f,(float)TOMATOsEngine::kMonitorHeight * 0.5f };
	Vector2 endPos = { TOMATOsEngine::kMonitorWidth * 0.5f,(float)TOMATOsEngine::kMonitorHeight * 0.5f };
	Vector2 pos = Easing::easing(scoreT, startPos, endPos, 0.01f, Easing::easeOutQuint, true);
	Vector2 minPos = { pos.x - TOMATOsEngine::kMonitorWidth * 0.5f,pos.y - TOMATOsEngine::kMonitorHeight * 0.5f - 40.0f };
	Vector2 maxPos = { pos.x + TOMATOsEngine::kMonitorWidth * 0.5f,pos.y + TOMATOsEngine::kMonitorHeight * 0.5f - 40.0f };
	TOMATOsEngine::DrawSpriteRect(minPos, maxPos, { 0.0f,0.0f }, { (float)TOMATOsEngine::kMonitorWidth ,(float)TOMATOsEngine::kMonitorHeight }, scoreTextureHandle_, 0xFFFFFFFF);
	if (scoreT >= 1.0f) {
		uint32_t randomNum = randomNumberGenerator_.NextUIntRange(100, 999);
		scoreFrame_++;
		if (scoreFrame_ >= 25) {
			if (scoreFrame_ == 25) {
				TOMATOsEngine::PlayAudio(scoreSoundHandle_);
			}
			DrawScoreNum(breakedBlockNum_, { 421.0f,357.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}
		else {
			DrawScoreNum(randomNum, { 421.0f,357.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}

		if (scoreFrame_ >= 50) {
			if (scoreFrame_ == 50) {
				TOMATOsEngine::PlayAudio(scoreSoundHandle_);
			}
			DrawScoreNum(combedStepNum_, { 324.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}
		else {
			DrawScoreNum(randomNum, { 324.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}

		if (scoreFrame_ >= 75) {
			if (scoreFrame_ == 75) {
				TOMATOsEngine::PlayAudio(scoreSoundHandle_);
			}
			DrawScoreNum(combedHrizonNum_, { 516.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}
		else {
			DrawScoreNum(randomNum, { 516.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}

		if (scoreFrame_ >= 100) {
			if (scoreFrame_ == 100) {
				TOMATOsEngine::PlayAudio(scoreSoundHandle_);
			}
			uint32_t score = breakedBlockNum_ + combedHrizonNum_ * kScoreHorizonMultiply + combedStepNum_ * kScoreStepMultiply;
			DrawScoreNum(score, { 503.0f,129.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}
		else {
			DrawScoreNum(randomNum, { 503.0f,129.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		}

	}
	else {
		uint32_t randomNum = randomNumberGenerator_.NextUIntRange(100, 999);
		DrawScoreNum(randomNum, { 421.0f,357.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		randomNum = randomNumberGenerator_.NextUIntRange(10, 99);
		DrawScoreNum(randomNum, { 324.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		randomNum = randomNumberGenerator_.NextUIntRange(10, 999);
		DrawScoreNum(randomNum, { 516.0f,244.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
		randomNum = randomNumberGenerator_.NextUIntRange(1000, 9999);
		DrawScoreNum(randomNum, { 503.0f,129.0f - 40.0f }, { 36.0f,36.0f }, { 36.0f,36.0f }, numTextureHandle_);
	}
}

void Field::DrawScoreNum(uint32_t num, Vector2 centerPos, Vector2 size, Vector2 textureSize, TextureHandle textureHandle) {
	uint32_t digit = Math::Digit(num);
	if (digit % 2 == 0) {
		int tmp = num;
		for (size_t i = 0; i < digit; i++) {
			Vector2 pos = { centerPos.x - size.x / 2.0f + size.x * i - (digit / 2.0f - 1) * size.x,  centerPos.y };
			int oneNum = static_cast<int>(tmp / (1 * std::pow(10, digit - i - 1)));
			tmp = tmp % static_cast<int>(1 * std::pow(10, digit - i - 1));
			TOMATOsEngine::DrawSpriteRectCenter(pos, size, { 36.0f * oneNum  ,0.0f }, textureSize, textureHandle, 0xFFFFFFFF);
		}
	}
	else {
		int tmp = num;
		for (size_t i = 0; i < digit; i++) {
			Vector2 pos = { centerPos.x + size.x * i - static_cast<int>(digit / 2.0f) * size.x, centerPos.y };
			int oneNum = static_cast<int>(tmp / (1 * std::pow(10, digit - i - 1)));
			tmp = tmp % static_cast<int>(1 * std::pow(10, digit - i - 1));
			TOMATOsEngine::DrawSpriteRectCenter(pos, size, { textureSize.x * oneNum ,0.0f }, textureSize, textureHandle, 0xFFFFFFFF);
		}
	}
}

void Field::DrawBlock() {

	//float blockEffectT = float(growCoolTime_) / growInterval_;
	// 危険域まで来たときに使用
	//float dangerousBlockSize = Math::Lerp(blockEffectT, 0.0f, kBlockSize);

	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		// ブロックの矩形座標
		Vector2 blockMinPos{}, blockMaxPos{};
		// ブロックの高さが危険域まで来ている
		bool isDangerHeight = blocks_[x][kDangerousBlockHeight] == BlockType::Normal;
		blockMinPos.x = float(x * kBlockSize);
		blockMaxPos.x = blockMinPos.x + float(kBlockSize);

		for (uint32_t y = 0; y < kNumVerticalBlocks; ++y) {
			blockMinPos.y = float(y * kBlockSize);
			blockMaxPos.y = blockMaxPos.y + float(kBlockSize);
			// 通常ブロック
			if (blocks_[x][y] == BlockType::Normal) {
				// 危険な高さまで来た
				if (isDangerHeight) {
					// 振れ幅
					float kAmplitude = 1.0f;
					float randomAmplitubeX = randomNumberGenerator_.NextFloatRange(-kAmplitude, kAmplitude);
					float randomAmplitubeY = randomNumberGenerator_.NextFloatRange(-kAmplitude, kAmplitude);
					blockMinPos += {randomAmplitubeX, randomAmplitubeY};
					blockMaxPos += {randomAmplitubeX, randomAmplitubeY};

					// 内側が赤くなる
					//Vector2 blockCenter = (blockMaxPos + blockMinPos) * 0.5f;
					//TOMATOsEngine::DrawSpriteRectCenter(blockCenter, { dangerousBlockSize, dangerousBlockSize }, {}, Vector2(32.0f, 32.0f), dangerousBlockTextureHandle_, 0xFFFFFFFF);
				}

				TOMATOsEngine::DrawSpriteRect(blockMinPos, blockMaxPos, {}, Vector2(32.0f, 32.0f), textureHandles_.at(Texture::kBlock), Color(blocksColor_[x][y]));
			}
			// フラッシュブロック
			else if (blocks_[x][y] == BlockType::Frash) {
				blockMinPos.x -= (kFrashTime - breakTime_) * 2.0f;
				blockMaxPos.x += (kFrashTime - breakTime_) * 2.0f;
				blockMinPos.y += (kFrashTime - breakTime_) * 2.0f;
				blockMaxPos.y -= (kFrashTime - breakTime_) * 2.0f;
				TOMATOsEngine::DrawRect(blockMinPos, blockMaxPos, 0xFFFFFFFF);
			}
		}
	}
	for (auto& block : gameOverBlocks_) {
		// 振れ幅
		float amplitude = 2.0f;
		Vector2 blockPos{};
		blockPos.x += block->position_.x + randomNumberGenerator_.NextFloatRange(-amplitude, amplitude);
		blockPos.y += block->position_.y + randomNumberGenerator_.NextFloatRange(-amplitude, amplitude);
		TOMATOsEngine::DrawSpriteRectAngle(blockPos, Vector2(32.0f, 32.0f), Vector2(0.5f, 0.5f), block->angle_, {}, Vector2(32.0f, 32.0f), textureHandles_.at(Texture::kGameOverBlock), Color(0.5f, 0.5f, 0.5f, 0.5f));
	}
	if (isTextDropping_) {
		//TOMATOsEngine::DrawSpriteRectAngle(gameOverPosition_, Vector2(320.0f, 240.0f), Vector2(0.5f, 0.5f), 0.0f, {}, Vector2(320.0f, 240.0f), textureHandles_.at(Texture::kGameOver), Color(0.5f, 0.5f, 0.5f, 0.5f));

	}
}

void Field::DrawGrow() {
	const uint32_t AnimationTime = 30;
	if (!isTextDropping_&&
		!isBlockBreaking_) {
		for (uint32_t i = 0; i < numGrowingBlocks_; i++) {
			Vector2 position = { static_cast<float>(nextBlockIndices_.at(i)) * static_cast<float>(kBlockSize) + 5.0f, static_cast<float>(kBlockSize) * -1.0f };
			Vector2 size = { 32.0f ,32.0f };
			// アニメーション
			growAnimationCount_++;
			if (growAnimationCount_ % AnimationTime == 0) {
				growAnimationCount_ = 0;
				growAnimationFrame_++;
				if (growAnimationFrame_ > growAnimationFrameSize_) {
					growAnimationFrame_ = 0;
				}
			}

			Vector2 texBase = { static_cast<float>(growAnimationFrame_) * 64.0f,0.0f };
			size.y = 32.0f * (std::clamp(float(growCoolTime_) / float(growInterval_), 0.0f, 32.0f));
			TOMATOsEngine::DrawRectAngle(position, size, Vector2(0.0f, 0.0f), 0.0f, 0xA2A2A2A2);
			TOMATOsEngine::DrawSpriteRectAngle(position, { 32.0f ,32.0f }, Vector2(0.0f, 0.0f), 0.0f, texBase, Vector2(64.0f, 64.0f), textureHandles_.at(Texture::kGrow), 0xFFFFFFFF);
		}
	}

}

void Field::GameOverUpdate() {
	if (isBlockBreaking_) {
		if (blockBleakAnimationCount_ <= 0) {
			Vector2 move{};
			const float speed_Min = 10.0f;
			const float speed_Max = 15.0f;
			size_t breakCount = 0;
			for (uint32_t x = 0; x < kNumHorizontalBlocks; x++) {
				if (blocks_[x][heightCount_] == BlockType::Normal) {
					blocks_[x][heightCount_] = BlockType::None;
					GameOver* gameOver = new GameOver();
					// ポジション
					gameOver->position_ = { float(x * kBlockSize) + float(kBlockSize) * 0.5f,float(heightCount_ * kBlockSize) + float(kBlockSize) * 0.5f };
					// 速度
					move.x = std::cos(randomNumberGenerator_.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
					move.y = std::sin(randomNumberGenerator_.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
					move.Normalize();
					float speed = randomNumberGenerator_.NextFloatRange(speed_Min, speed_Max);
					gameOver->velocity_ = move * speed;
					// 角度
					gameOver->angle_ = 0.0f;
					gameOver->addAngle_ = randomNumberGenerator_.NextFloatRange(10.0f * Math::ToRadian, 30.0f * Math::ToRadian);
					if (randomNumberGenerator_.NextUIntLimit() % 2 == 0) {
						gameOver->addAngle_ *= -1;
					}
					gameOver->isDrop_ = false;
					gameOver->isCount_ = false;
					gameOverBlocks_.emplace_back(gameOver);
					blocksColor_[x][heightCount_] = { 0.5f,0.5f,0.5f,0.8f };
					++breakCount;
				}
			}

			size_t playHandle = TOMATOsEngine::PlayAudio(blockDownHandle_);
			TOMATOsEngine::SetVolume(playHandle, 0.5f);

			// 高さが0になったらゲームオーバーフラグを立てる
			if (heightCount_ < 0) {
				heightCount_ = kDeathLine_;
				isTextDropping_ = true;
				isBlockBreaking_ = false;
			}
			// 高さを一段下げる
			heightCount_--;
			// アニメーションカウントリセット
			const uint32_t kCoolTime = 10;
			blockBleakAnimationCount_ = kCoolTime;
		}
		else {
			blockBleakAnimationCount_--;
		}
	}
	else if (isTextDropping_) {
		/*const uint32_t kDropTextTime = 120;*/
		const float kGravity = -1.0f;
		gameOverPosition_.y -= 10.0f;
		if (gameOverBlockCount_ >= gameOverBlocks_.size() && gameOverPosition_.y <= 0.0f) {
			isVanish_ = true;
		}

		bool playAudio = false;

		for (auto& block : gameOverBlocks_) {
			if (!block->isDrop_ && block->position_.y > gameOverPosition_.y - float(TOMATOsEngine::kMonitorHeight) * 0.5f) {
				block->isDrop_ = true;
				if (!playAudio) {
					size_t playHandle = TOMATOsEngine::PlayAudio(blockDown2Handle_);
					TOMATOsEngine::SetVolume(playHandle, 0.4f);
					playAudio = true;
				}
			}
			else if (block->isDrop_) {
				block->velocity_.y += kGravity;
				block->acceleration_ += block->velocity_;
				block->position_ += block->acceleration_;
				block->acceleration_ = { 0.0f,0.0f };
				block->angle_ += block->addAngle_;
				if (!block->isCount_ && block->position_.y < 0.0f - 32.0f) {
					block->isCount_ = true;

					gameOverBlockCount_++;
				}
			}
		}

	}
}


void Field::ColorClearBlock() {
	if (!isInGameOver_) {
		for (uint32_t x = 0; x < kNumVerticalBlocks; x++) {
			for (uint32_t y = 0; y < kNumHorizontalBlocks; y++) {
				if (blocks_[x][y] == BlockType::Normal) {
					blocksColor_[x][y] = initializeColor_;
				}
			}
		}
	}
}

void Field::SetColorBlock(uint32_t blockIndexX, uint32_t blockIndexY, const Vector4& color) {
	assert(IsInField(blockIndexX, blockIndexY));
	blocksColor_[blockIndexX][blockIndexY] = color;
}
void Field::BreakBlock(uint32_t blockIndexX, uint32_t blockIndexY) {
	assert(IsInField(blockIndexX, blockIndexY));
	FeverManager* fever = FeverManager::GetInstance();
	if (blocks_[blockIndexX][blockIndexY] == BlockType::Normal) {
		blocks_[blockIndexX][blockIndexY] = BlockType::None;
		breakedBlockNum_++;
		// フィーバー用の壊れたブロックを取得
		if (!fever->GetIsFever()) {
			fever->IncreaseBlock(1);
		}
	}

	// 音
	size_t playHandle = TOMATOsEngine::PlayAudio(breakSoundHandle_);
	TOMATOsEngine::SetPitch(playHandle, 1.0f);
}

void Field::BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY, bool isHorizontal) {
	assert(IsInField(blockIndexX, blockIndexY));
	blockIndexX;
	FeverManager* fever = FeverManager::GetInstance();
	for (size_t x = 0; x < kNumHorizontalBlocks; x++) {
		breakTime_ = kFrashTime;
		if (blocks_[static_cast<uint32_t>(x)][blockIndexY] == BlockType::Normal) {
			breakedBlockNum_++;
			// フィーバー用の壊れたブロックを取得
			if (!fever->GetIsFever()) {
				if (isHorizontal == true) {
					fever->IncreaseBlock(1);
				}
				else {
					fever->IncreaseBlock(2);
				}

			}
		}
		blocks_[static_cast<uint32_t>(x)][blockIndexY] = BlockType::Frash;
	}
	downBlockIndex_ = blockIndexY + 1;
	isFlash_ = true;
	if (isHorizontal) {
		combedHrizonNum_++;
	}
	else {
		combedStepNum_++;
	}

	// 音 
	size_t playHandle = TOMATOsEngine::PlayAudio(lineBreakSoundHandle_);
	TOMATOsEngine::SetVolume(playHandle, 1.2f);
}

void Field::ClearBreakBlockHorizon() {
	int32_t y = GetHeightestIndex();
	if (y != -1) {
		// 音 
		if (isClearFlash_ == false) {
			size_t playHandle = TOMATOsEngine::PlayAudio(lineBreakSoundHandle_);
			TOMATOsEngine::SetVolume(playHandle, 1.2f);

			for (size_t x = 0; x < kNumHorizontalBlocks; x++) {
				if (blocks_[static_cast<uint32_t>(x)][y] == BlockType::Normal) {
					blocks_[static_cast<uint32_t>(x)][y] = BlockType::Frash;
					breakTime_ = kFrashTime;
					isClearFlash_ = true;
				}
			}
		}
	}
	else {
		if (isClearFlash_ == false) {
			isVanish_ = true;
		}
	}
}

void Field::DownBlockHorizon() {
	// 一列下げる
	for (size_t x = 0; x < kNumHorizontalBlocks; x++) {
		for (size_t y = downBlockIndex_; y < kNumVerticalBlocks; y++) {
			blocks_[static_cast<uint32_t>(x)][static_cast<uint32_t>(y - 1)] = blocks_[static_cast<uint32_t>(x)][static_cast<uint32_t>(y)];
		}
	}
}

uint32_t Field::CalcBlockIndexX(float worldPosX) const {
	float pX = std::clamp(worldPosX, 0.0f, fieldSize_.x);
	return uint32_t(pX / kBlockSize);
}

uint32_t Field::CalcBlockIndexY(float worldPosY) const {
	float pY = std::clamp(worldPosY, 0.0f, fieldSize_.y);
	return uint32_t(pY / kBlockSize);
}

Field::BlockType Field::GetBlock(const Vector2& worldPosition) const {
	return blocks_[CalcBlockIndexX(worldPosition.x)][CalcBlockIndexY(worldPosition.y)];
}

Field::BlockType Field::GetBlock(uint32_t worldPosX, uint32_t worldPosY) const {
	return blocks_[worldPosX][worldPosY];
}

float Field::GetBlockLeft(uint32_t blockIndexX) const {
	return float(blockIndexX * kBlockSize);
}
float Field::GetBlockTop(uint32_t blockIndexY) const {
	return float(blockIndexY * kBlockSize + kBlockSize);

}
float Field::GetBlockRight(uint32_t blockIndexX) const {
	return float(blockIndexX * kBlockSize + kBlockSize);

}
float Field::GetBlockBottom(uint32_t blockIndexY) const {
	return float(blockIndexY * kBlockSize);
}

bool Field::IsInField(const Vector2& worldPosition) const {
	return IsInField(uint32_t(worldPosition.x / kBlockSize), uint32_t(worldPosition.y / kBlockSize));
}

bool Field::IsInField(uint32_t blockIndexX, uint32_t blockIndexY) const {
	if (blockIndexX >= 0 && blockIndexX < kNumHorizontalBlocks &&
		blockIndexY >= 0 && blockIndexY < kNumVerticalBlocks) {
		return true;
	}
	return false;
}

void Field::Edit() {
#ifdef _DEBUG

	ImGui::Begin("Field");
	int coolTime = int(growCoolTime_);
	ImGui::SliderInt("GrowCoolTime", &coolTime, 0, int(growInterval_));
	growCoolTime_ = uint32_t(coolTime);

	int interval = int(growInterval_);
	ImGui::SliderInt("GrowInterbal", &interval, 0, 500);
	growInterval_ = uint32_t(interval);

	int numB = int(numGrowingBlocks_);
	ImGui::SliderInt("NumGrowingBlocks", &numB, 0, int(kNumHorizontalBlocks - 1));
	numGrowingBlocks_ = uint32_t(numB);
	ImGui::End();
#endif // _DEBUG
}

void Field::ChackBlock() {
	for (uint32_t x = 0; x < kNumHorizontalBlocks; x++) {
		if (blocks_[x][kDeathLine_] == BlockType::Normal) {
			isInGameOver_ = true;
			isBlockBreaking_ = true;
			ColorClearBlock();
			return;
		}
	}
}

void Field::GrowField(uint32_t numBlocks) {
	assert(numBlocks > 0 && numBlocks < kNumHorizontalBlocks);

	// 0~横幅分の数字配列をシャッフルして
	// numBlocks以下の配列要素のインデックスを成長させる
	std::vector<uint32_t> blockIndices(kNumHorizontalBlocks);
	std::iota(blockIndices.begin(), blockIndices.end(), 0);

	// シャッフルする
	{
		uint32_t i = uint32_t(blockIndices.size() - 1);
		while (i > 0) {
			uint32_t j = randomNumberGenerator_.NextUIntRange(0, i);

			std::swap(blockIndices[i], blockIndices[j]);

			i--;
		}
	}

	for (uint32_t i = 0; i < numBlocks; ++i) {
		// 成長
		Grow(blockIndices[i]);
	}
}

std::vector<uint32_t> Field::GetGrowField(uint32_t numBlocks) {
	assert(numBlocks > 0 && numBlocks < kNumHorizontalBlocks);
	numBlocks;
	// 0~横幅分の数字配列をシャッフルして
	// numBlocks以下の配列要素のインデックスを成長させる
	std::vector<uint32_t> blockIndices(kNumHorizontalBlocks);
	std::iota(blockIndices.begin(), blockIndices.end(), 0);

	// シャッフルする
	{
		uint32_t i = uint32_t(blockIndices.size() - 1);
		while (i > 0) {
			uint32_t j = randomNumberGenerator_.NextUIntRange(0, i);

			std::swap(blockIndices[i], blockIndices[j]);

			i--;
		}

		uint32_t leatestindex = GetLeatestIndex();

		auto leatestIndex = std::find(blockIndices.begin(), blockIndices.end(), leatestindex);
		std::swap(blockIndices[0], *leatestIndex);

		leatestindex = GetLeatestIndex(blockIndices[0]);

		leatestIndex = std::find(blockIndices.begin(), blockIndices.end(), leatestindex);
		std::swap(blockIndices[1], *leatestIndex);
	}
	return blockIndices;
}

void Field::Grow(uint32_t horizontalIndex) {
	auto block = blocks_[horizontalIndex];
	for (uint32_t i = kNumVerticalBlocks - 1; i > 0; --i) {
		uint32_t j = i - 1;
		if (block[j] != BlockType::None) {
			block[i] = block[j];
		}
	}
	block[0] = BlockType::Normal;

	blocksColor_[horizontalIndex][0] = initializeColor_;
}

void Field::SetGrow(std::vector<uint32_t> blockIndices, uint32_t numBlocks) {
	for (uint32_t i = 0; i < numBlocks; ++i) {
		// 成長
		Grow(blockIndices[i]);
	}
}

int32_t  Field::GetHeightestIndex() {
	int32_t  heightestIndex = -1;
	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		for (int32_t y = 0; y < kNumVerticalBlocks; ++y) {
			if (blocks_[x][y] == BlockType::Normal && heightestIndex < y) {
				heightestIndex = y;
			}
		}
	}
	return heightestIndex;
}

int32_t Field::GetHeightestIndex(uint32_t xIndex)
{
    int32_t  heightestIndex = -1;
    
        for (int32_t y = 0; y < kNumVerticalBlocks; ++y) {
            if (blocks_[xIndex][y] == BlockType::Normal && heightestIndex < y) {
                heightestIndex = y;
            }
        }
    
    return heightestIndex;
}

uint32_t  Field::GetLeatestIndex() {
	uint32_t  leatestYIndex = kNumVerticalBlocks;
	uint32_t  leatestXIndex[kNumHorizontalBlocks];
	bool allNoneBlock = true;
	uint32_t result = 0;
	bool isLeatestIsNoneBlock = false;
	for (int i = 0; i < kNumHorizontalBlocks; i++) {
		leatestXIndex[i] = 0;
	}
	int inNum = 0;
	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		if (blocks_[x][0] == BlockType::None) {
			isLeatestIsNoneBlock = true;
			break;
		}
		for (uint32_t y = kNumVerticalBlocks - 1; y < kNumVerticalBlocks; --y) {
			if (blocks_[x][y] == BlockType::Normal && leatestYIndex > y) {
				leatestYIndex = y;
				allNoneBlock = false;
				break;
			}
			else if (blocks_[x][y] == BlockType::Normal) {
				break;
			}
		}
	}

	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		if (isLeatestIsNoneBlock == true) {
			if (blocks_[x][0] == BlockType::None) {
				leatestXIndex[inNum] = x;
				inNum++;
			}
		}
		else {
			for (uint32_t y = kNumVerticalBlocks - 1; y < kNumVerticalBlocks; --y) {
				if (blocks_[x][y] == BlockType::Normal && y > leatestYIndex) {
					break;
				}
				else if (blocks_[x][y] == BlockType::Normal && y == leatestYIndex) {
					leatestXIndex[inNum] = x;
					inNum++;
					break;
				}
			}
		}
	}

	result = leatestXIndex[randomNumberGenerator_.NextUIntRange(0, inNum - 1)];

	if (allNoneBlock == true) {
		result = randomNumberGenerator_.NextUIntRange(0, kNumHorizontalBlocks - 1);
	}

	return result;
}

uint32_t  Field::GetLeatestIndex(uint32_t exclusion) {
	uint32_t  leatestYIndex = kNumVerticalBlocks;
	uint32_t  leatestXIndex[kNumHorizontalBlocks];
	bool allNoneBlock = true;
	uint32_t result = 0;
	bool isLeatestIsNoneBlock = false;
	for (int i = 0; i < kNumHorizontalBlocks; i++) {
		leatestXIndex[i] = 0;
	}
	int inNum = 0;
	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		if (x != exclusion) {
			if (blocks_[x][0] == BlockType::None) {
				isLeatestIsNoneBlock = true;
				break;
			}
			for (uint32_t y = kNumVerticalBlocks - 1; y < kNumVerticalBlocks; --y) {
				if (blocks_[x][y] == BlockType::Normal && leatestYIndex > y) {
					leatestYIndex = y;
					allNoneBlock = false;
					break;
				}
				else if (blocks_[x][y] == BlockType::Normal) {
					break;
				}
			}
		}
	}

	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		if (x != exclusion) {
			if (isLeatestIsNoneBlock == true) {
				if (blocks_[x][0] == BlockType::None) {
					leatestXIndex[inNum] = x;
					inNum++;
				}
			}
			else {
				for (uint32_t y = kNumVerticalBlocks - 1; y < kNumVerticalBlocks; --y) {
					if (blocks_[x][y] == BlockType::Normal && y > leatestYIndex) {
						break;
					}
					else if (blocks_[x][y] == BlockType::Normal && y == leatestYIndex) {
						leatestXIndex[inNum] = x;
						inNum++;
						break;
					}
				}
			}
		}
	}

	result = leatestXIndex[randomNumberGenerator_.NextUIntRange(0, inNum - 1)];

	if (allNoneBlock == true) {
		result = randomNumberGenerator_.NextUIntRange(0, kNumHorizontalBlocks - 1);
	}

	return result;
}

void Field::HarryEffect() {
	isDangerousHeight_ = false;
	// ブロックが高くなってくるとBGMテンポアップ
	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		if (blocks_[x][kDangerousBlockHeight] != BlockType::None) {
			isDangerousHeight_ = true;
			break;
		}
	}
}
