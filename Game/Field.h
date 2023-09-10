#pragma once

#include <cstdint>

#include "TextureHandle.h"
#include "Math/MathUtils.h"
#include "Math/Random.h"

class ParticleManager;

class Field {
public:
    static const uint32_t kBlockSize = 40;
    static const uint32_t kNumHorizontalBlocks = 16;
    static const uint32_t kNumVerticalBlocks = 24;
    static const uint32_t kGrowInterval = 120;
    static const uint32_t kNumGrowingBlocks = 4;

    enum BlockType {
        None,
        Normal,

        NumTypes
    };

    void Initialize();
    void Update();
    void Draw();

    void ColorClearBlock();
    void SetColorBlock(uint32_t blockIndexX, uint32_t blockIndexY, const Vector4& color);
    void BreakBlock(uint32_t blockIndexX, uint32_t blockIndexY);
    void BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY);

    uint32_t CalcBlockIndexX(float worldPosX) const;
    uint32_t CalcBlockIndexY(float worldPosY) const;
    BlockType GetBlock(const Vector2& worldPosition) const;
    BlockType GetBlock(uint32_t blockIndexX, uint32_t blockIndexY) const;
   
    float GetBlockLeft(uint32_t blockIndexX) const;
    float GetBlockTop(uint32_t blockIndexY) const;
    float GetBlockRight(uint32_t blockIndexX) const;
    float GetBlockBottom(uint32_t blockIndexY) const;

    const Vector2& GetSize() const { return fieldSize_; }

    bool IsInField(const Vector2& worldPosition) const;
    bool IsInField(uint32_t blockIndexX, uint32_t blockIndexY) const;

    void Edit();

    void SetParticleManager(ParticleManager* particleManager) {particleManager_ = particleManager;}
private:
    void GrowField(uint32_t numBlocks);
    void Grow(uint32_t horizontalIndex);

    // 縦に伸びるので横縦配置
    // 左から右
    // 下から上
    BlockType blocks_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    Vector2 fieldSize_;
    // 色
    Vector4 blocksColor_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    Vector4 initializeColor_;
    // 伸びるまでの時間
    uint32_t growCoolTime_ = 0;
    uint32_t growInterval_ = 120;
    uint32_t numGrowingBlocks_ = 4;
    // 乱数生成器
    Random::RandomNumberGenerator randomNumberGenerator_;
    // パーティクルマネージャー
    ParticleManager* particleManager_;
    // テクスチャハンドル
    TextureHandle textureHandle_;
};