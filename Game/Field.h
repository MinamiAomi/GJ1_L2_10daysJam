#pragma once

#include <array>
#include <cstdint>
#include <memory>

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
    static const uint32_t kFrashTime = 10;

    enum BlockType {
        None,
        Normal,
        Frash,
        GameOverBlock,

        NumTypes
    };

    enum Texture {
        kBlock,
        kGrow,
        kGameOverBlock,
        kGameOver,
        kDeadLine,


        Count,
    };

    void Initialize();
    void Update();
    void Draw();

    void ColorClearBlock();
    void SetColorBlock(uint32_t blockIndexX, uint32_t blockIndexY, const Vector4& color);
    void BreakBlock(uint32_t blockIndexX, uint32_t blockIndexY);
    void BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY);
    void ClearBreakBlockHorizon();

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
    void DownBlockHorizon();

    Vector2 GetGameOverPosition() { return gameOverPosition_; }
    bool GetIsGameOver() { return isGameOver_; }
    bool GetIsInGameOver() { return isInGameOver_; }
    bool GetIsVanish() { return isVanish_; }

private:
    struct GameOver {
        Vector2 position_;
        Vector2 velocity_;
        Vector2 acceleration_;
        float angle_;
        float addAngle_;
        bool isDrop_;
        bool isCount_;
    };

    void ChackBlock();
    void GrowField(uint32_t numBlocks);
    void Grow(uint32_t horizontalIndex);
    // nextBlockIndices_を使って制御
    std::vector<uint32_t> GetGrowField(uint32_t numBlocks);
    void SetGrow(std::vector<uint32_t> horizontalIndex, uint32_t numBlocks);
    void DrawBlock();
    void DrawGrow();
    int32_t GetHeightestIndex();

    void GameOverUpdate();
    // 縦に伸びるので横縦配置
    // 左から右
    // 下から上
    BlockType blocks_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    //
    BlockType drawBlocks_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    Vector2 fieldSize_;
    // 色
    Vector4 blocksColor_[kNumHorizontalBlocks][kNumVerticalBlocks]{};
    Vector4 initializeColor_;
    // 伸びるまでの時間
    uint32_t growCoolTime_ = 0;
    uint32_t growInterval_ = 120;
    uint32_t numGrowingBlocks_ = 4;
    // 次の成長を保持
    std::vector<uint32_t> nextBlockIndices_;
    // 乱数生成器
    Random::RandomNumberGenerator randomNumberGenerator_;
    // パーティクルマネージャー
    ParticleManager* particleManager_;
    // テクスチャハンドル
    static const uint32_t  TextureMax = Texture::Count;
    std::array<TextureHandle, TextureMax> textureHandles_;
    // 予想テクスチャアニメーション
    uint32_t growAnimationCount_;
    uint32_t growAnimationFrame_;
    uint32_t growAnimationFrameSize_;
    //時止める変数
    uint32_t breakTime_;
    uint32_t downBlockIndex_;
    bool isFlash_;
    bool isClearFlash_;

    // オーディオ類
    size_t breakSoundHandle_ = 0;
    size_t lineBreakSoundHandle_ = 0;

    // デットライン
    Vector2 deadLinePosition_;
    Vector2 deadLineSize_;
    float deadLineAnimationFront_;
    float deadLineAnimationBack_;
    float deadLineColorH_;

    // ゲームオーバー
    const uint32_t kDeathLine_ = 10;
    uint32_t blockBleakAnimationCount_;
    int32_t heightCount_;

    //　ゲームクリア
    uint32_t clearLine_;
    bool isVanish_;

    // マップチップの位置を保存
    std::vector<std::unique_ptr<GameOver>> gameOverBlocks_;
    Vector2 gameOverPosition_;
    Vector2 gameOverPositionStart_;
    Vector2 gameOverPositionEnd_;
    uint32_t dropTextCount_;
    uint32_t gameOverBlockCount_;

    bool isBlockBreaking_;
    bool isTextDropping_;
    bool isInGameOver_;
    bool isGameOver_;
};