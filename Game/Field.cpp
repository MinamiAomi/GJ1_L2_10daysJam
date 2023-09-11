#include "Field.h"

#include <numeric>

#include "TOMATOsEngine.h"
#include "Particle/ParticleManager.h"
#include "Math/Color.h"

void Field::Initialize() {
	memset(blocks_, BlockType::None, sizeof(blocks_));

	fieldSize_ = { float(kBlockSize * kNumHorizontalBlocks), float(kBlockSize * kNumVerticalBlocks) };

	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
	GrowField(5);
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

	heightCount_ = kDeathLine_;
	blockBleakAnimationCount_ = 0;
	gameOverBlocks_.clear();

	gameOverPosition_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,float(TOMATOsEngine::kMonitorHeight) + float(TOMATOsEngine::kMonitorHeight) * 0.5f };
	gameOverPositionStart_ = gameOverPosition_;
	gameOverPositionEnd_ = { gameOverPosition_ .x,float(TOMATOsEngine::kMonitorHeight) * 0.5f };
	dropTextCount_ = 0;

	isBlockBreaking_ = false;
	isTextDropping_ = false;
	isGameOver_ = false;
	isInGameOver_ = false;
  
  
    breakSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/break1.wav");
    lineBreakSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/lineBreak.wav");
}

void Field::Update() {
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
	}
	else {
		GameOverUpdate();
	}
}

void Field::Draw() {
    TOMATOsEngine::SetBlendMode(kBlendModeNormal);
    DrawBlock();
    DrawGrow();
}

void Field::DrawBlock() {
	for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
		// ブロックの矩形座標
		Vector2 blockMinPos{}, blockMaxPos{};
		for (uint32_t y = 0; y < kNumVerticalBlocks; ++y) {
			blockMinPos.x = float(x * kBlockSize);
			blockMaxPos.x = blockMinPos.x + float(kBlockSize);
			blockMinPos.y = float(y * kBlockSize);
			blockMaxPos.y = blockMaxPos.y + float(kBlockSize);
			// 通常ブロック
			if (blocks_[x][y] == BlockType::Normal) {
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
		Random::RandomNumberGenerator rnd{};
		float distance = 2.0f;
		Vector2 blockPos{};
		blockPos.x += block->position_.x + rnd.NextFloatRange(-distance, distance);
		blockPos.y += block->position_.y + rnd.NextFloatRange(-distance, distance);
		TOMATOsEngine::DrawSpriteRectAngle(blockPos, Vector2(32.0f, 32.0f), Vector2(0.5f, 0.5f), block->angle_, {}, Vector2(32.0f, 32.0f), textureHandles_.at(Texture::kGameOverBlock), Color(0.5f,0.5f,0.5f,0.5f));
	}
	if (isTextDropping_) {
		TOMATOsEngine::DrawSpriteRectAngle(gameOverPosition_, Vector2(320.0f, 240.0f), Vector2(0.5f, 0.5f),0.0f, {}, Vector2(320.0f, 240.0f), textureHandles_.at(Texture::kGameOverBlock), Color(0.5f,0.5f,0.5f,0.5f));

	}
}

void Field::DrawGrow() {
	const uint32_t AnimationTime = 30;
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
		TOMATOsEngine::DrawSpriteRectAngle(position, size, Vector2(0.0f, 0.0f), 0.0f, texBase, Vector2(64.0f, 64.0f), textureHandles_.at(Texture::kGrow), 0xFFFFFFFF);
	}

}

void Field::GameOverUpdate() {
	if (isBlockBreaking_) {
		if (blockBleakAnimationCount_ <= 0) {
			Random::RandomNumberGenerator rnd{};
			Vector2 move{};
			const float speed_Min = 10.0f;
			const float speed_Max = 15.0f;
			for (uint32_t x = 0; x < kNumHorizontalBlocks; x++) {
				if (blocks_[x][heightCount_] == BlockType::Normal) {
					blocks_[x][heightCount_] = BlockType::None;
					GameOver* gameOver=new GameOver();
					// ポジション
					gameOver->position_ = { float(x * kBlockSize) + float(kBlockSize) * 0.5f,float(heightCount_ * kBlockSize) + float(kBlockSize) * 0.5f };
					// 速度
					move.x = std::cos(rnd.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
					move.y = std::sin(rnd.NextFloatRange(30.0f * Math::ToRadian, 150.0f * Math::ToRadian));
					move.Normalize();
					float speed = rnd.NextFloatRange(speed_Min, speed_Max);
					gameOver->velocity_ = move * speed;
					// 角度
					gameOver->angle_ = 0.0f;
					gameOver->addAngle_ = rnd.NextFloatRange(10.0f * Math::ToRadian, 30.0f * Math::ToRadian);
					if (rnd.NextUIntLimit() % 2 == 0) {
						gameOver->addAngle_ *= -1;
					}
					gameOver->isDrop_ = false;
					gameOverBlocks_.emplace_back(gameOver);
					blocksColor_[x][heightCount_] = { 0.5f,0.5f,0.5f,0.8f };
				}
			}
			// 高さが0になったらゲームオーバーフラグを立てる
			if (heightCount_ < 0) {
				heightCount_ = kDeathLine_;
				isTextDropping_ = true;
				isBlockBreaking_ = false;
			}
			// 高さを一段下げる
			heightCount_--;
			// アニメーションカウントリセット
			const uint32_t kCoolTime = 15;
			blockBleakAnimationCount_ = kCoolTime;
		}
		else {
			blockBleakAnimationCount_--;
		}
	}
	else if (isTextDropping_) {
		const uint32_t kDropTextTime = 300;
		const float kGravity = -1.0f;
		if (dropTextCount_ >= kDropTextTime) {
			/*isGameOver_ = true;*/
		}
		dropTextCount_++;
		float t = std::clamp(float(dropTextCount_) / float(kDropTextTime), 0.0f, 1.0f);

		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		if (t > 0.5f) {	
			t = ((4.0f * t - 5.0f)*(4.0f * t - 4.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2;
		}

		gameOverPosition_.y = Math::Lerp(t, gameOverPositionStart_.y, gameOverPositionEnd_.y);

		for (auto& block : gameOverBlocks_) {
			if (!block->isDrop_ && block->position_.y > gameOverPosition_.y) {
				block->isDrop_ = true;
			}
			else if(block->isDrop_){
				block->velocity_.y += kGravity;
				block->acceleration_ += block->velocity_;
				block->position_ += block->acceleration_;
				block->acceleration_ = { 0.0f,0.0f };
				block->angle_ += block->addAngle_;
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
	if (blocks_[blockIndexX][blockIndexY] == BlockType::Normal) {
		blocks_[blockIndexX][blockIndexY] = BlockType::None;
	}

    // 音
    size_t playHandle = TOMATOsEngine::PlayAudio(breakSoundHandle_);
    TOMATOsEngine::SetPitch(playHandle, 1.0f);
}

void Field::BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY) {
    assert(IsInField(blockIndexX, blockIndexY));
    for (size_t x = 0; x < kNumHorizontalBlocks; x++) {
        breakTime_ = kFrashTime;
      /*  if (blocks_[static_cast<uint32_t>(x)][blockIndexY] == BlockType::Normal) {
            blocks_[static_cast<uint32_t>(x)][blockIndexY] = BlockType::Frash;

        }*/
		blocks_[static_cast<uint32_t>(x)][blockIndexY] = BlockType::Frash;
    }
    downBlockIndex_ = blockIndexY + 1;
    isFlash_ = true;

    // 音 
    size_t playHandle = TOMATOsEngine::PlayAudio(lineBreakSoundHandle_);
    TOMATOsEngine::SetVolume(playHandle, 1.2f);
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