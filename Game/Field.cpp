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
	for (uint32_t x = 0; x < kNumVerticalBlocks; x++) {
		for (uint32_t y = 0; y < kNumHorizontalBlocks; y++) {
			if (blocks_[x][y] == BlockType::Normal) {
				blocksColor_[x][y] = initializeColor_;
			}
		}
	}
	growCoolTime_ = 0;

	growAnimationCount_ = 0;
	growAnimationFrame_ = 0;
	growAnimationFrameSize_=4;

	breakTime_ = 0;
	downBlockIndex_ = 0;
	isFlash_ = false;

	textureHandles_.at(Texture::kBlock) = TOMATOsEngine::LoadTexture("Resources/block.png");
	textureHandles_.at(Texture::kGrow) = TOMATOsEngine::LoadTexture("Resources/grow.png");
}

void Field::Update() {
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
			if (blocks_[x][y] == BlockType::Frash) {
				blockMinPos.x -= (kFrashTime - breakTime_) * 1.5f;
				blockMaxPos.x += (kFrashTime - breakTime_) * 1.5f;
				blockMinPos.y += (kFrashTime - breakTime_) * 1.5f;
				blockMaxPos.y -= (kFrashTime - breakTime_) * 1.5f;
				TOMATOsEngine::DrawRect(blockMinPos, blockMaxPos,0xFFFFFFFF);
			}
		}
	}
}

void Field::DrawGrow() {
	const uint32_t AnimationTime = 30;
	for (uint32_t i = 0; i < numGrowingBlocks_; i++) {
		Vector2 position = { static_cast<float>(nextBlockIndices_.at(i)) * static_cast<float>(kBlockSize) + 5.0f, static_cast<float>(kBlockSize)*-1.0f };
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


void Field::ColorClearBlock() {
	for (uint32_t x = 0; x < kNumVerticalBlocks; x++) {
		for (uint32_t y = 0; y < kNumHorizontalBlocks; y++) {
			if (blocks_[x][y] == BlockType::Normal) {
				blocksColor_[x][y] = initializeColor_;
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
	blocks_[blockIndexX][blockIndexY] = BlockType::None;
}

void Field::BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY) {
	assert(IsInField(blockIndexX, blockIndexY));
	for (size_t x = 0; x < kNumHorizontalBlocks; x++) {
		breakTime_ = kFrashTime;
		if (blocks_[static_cast<uint32_t>(x)][blockIndexY] == BlockType::Normal) {
			blocks_[static_cast<uint32_t>(x)][blockIndexY] = BlockType::Frash;
		}
	}
	downBlockIndex_ = blockIndexY + 1;
	isFlash_ = true;
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