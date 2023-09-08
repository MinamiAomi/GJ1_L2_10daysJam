#pragma once

#include <cstdint>

#include "Math/MathUtils.h"
#include "Math/Random.h"

class Field {
public:
    static const uint32_t kBlockSize = 40;
    static const uint32_t kNumHorizontalBlocks = 9;
    static const uint32_t kNumVerticalBlocks = 12;
    static const uint32_t kGrowInterval = 120;
    static const uint32_t kNumGrowingBlocks = 4;

    enum BlockType : unsigned char {
        None,
        Normal,

        NumTypes
    };

    void Initialize();
    void Update();
    void Draw();

    uint32_t CalcBlockIndexX(float worldPosX) const;
    uint32_t CalcBlockIndexY(float worldPosY) const;
    BlockType GetBlock(const Vector2& worldPosition) const;
    BlockType GetBlock(uint32_t blockIndexX, uint32_t blockIndexY) const;
   
    float GetBlockLeft(uint32_t blockIndexX) const;
    float GetBlockTop(uint32_t blockIndexY) const;
    float GetBlockRight(uint32_t blockIndexX) const;
    float GetBlockBottom(uint32_t blockIndexY) const;

    const Vector2& GetSize() const { return fieldSize_; }

private:
    void GrowField(uint32_t numBlocks);
    void Grow(uint32_t horizontalIndex);

    // 縦に伸びるので横縦配置
    // 左から右
    // 下から上
    BlockType blocks_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    Vector2 fieldSize_;
    // 伸びるまでの時間
    uint32_t growCoolTime_ = 0;
    
    // 乱数生成器
    Random::RandomNumberGenerator randomNumberGenerator_;
};