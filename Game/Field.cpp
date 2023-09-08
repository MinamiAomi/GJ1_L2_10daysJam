#include "Field.h"

#include <numeric>

#include "TOMATOsEngine.h"

void Field::Initialize() {
    memset(blocks_, BlockType::None, sizeof(blocks_));

    fieldSize_ = { float(kBlockSize * kNumHorizontalBlocks), float(kBlockSize * kNumVerticalBlocks) };

    GrowField(5);
    GrowField(5);
    GrowField(5);
    GrowField(5);

    growCoolTime_ = 0;
}

void Field::Update() {
    ++growCoolTime_;
    if (growCoolTime_ >= kGrowInterval) {
        GrowField(kNumGrowingBlocks);
        growCoolTime_ = 0;
    }
}

void Field::Draw() {

    TOMATOsEngine::DrawRect({}, fieldSize_, 0xFFFFFFFF);

    for (uint32_t x = 0; x < kNumHorizontalBlocks; ++x) {
        // ブロックの矩形座標
        Vector2 blockMinPos{}, blockMaxPos{};
        blockMinPos.x = float(x * kBlockSize);
        blockMaxPos.x = blockMinPos.x + float(kBlockSize);

        for (uint32_t y = 0; y < kNumVerticalBlocks; ++y) {
            blockMinPos.y = float(y * kBlockSize);
            blockMaxPos.y = blockMaxPos.y + float(kBlockSize);
            // 通常ブロック
            if (blocks_[x][y] == BlockType::Normal) {

                TOMATOsEngine::DrawRect(blockMinPos, blockMaxPos, 0xFF666666);
            }
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

void Field::Grow(uint32_t horizontalIndex) {
    auto block = blocks_[horizontalIndex];
    for (uint32_t i = kNumVerticalBlocks - 1; i > 0; --i) {
        uint32_t j = i - 1;
        if (block[j] != BlockType::None) {
            block[i] = block[j];
        }
    }
    block[0] = BlockType::Normal;
}
