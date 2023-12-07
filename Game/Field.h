#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "TextureHandle.h"
#include "Math/MathUtils.h"
#include "Math/Random.h"

class ParticleManager;
class Player;

class Field {
public:
    static const uint32_t kBlockSize = 40;
    static const uint32_t kNumHorizontalBlocks = 16;
    static const uint32_t kNumVerticalBlocks = 24;
    static const uint32_t kGrowInterval = 120;
    static const uint32_t kNumGrowingBlocks = 4;
    static const uint32_t kFrashTime = 10;
	static const uint32_t kDangerousBlockHeight = 7;
    static const uint32_t kScoreStepMultiply = 20;
    static const uint32_t kScoreHorizonMultiply = 10;

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

    void DrawScore();

    void ColorClearBlock();
    void SetColorBlock(uint32_t blockIndexX, uint32_t blockIndexY, const Vector4& color);
    void BreakBlock(uint32_t blockIndexX, uint32_t blockIndexY);
    void BreakBlockHorizon(uint32_t blockIndexX, uint32_t blockIndexY, bool isHorizontal);
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

    uint32_t GetBreakedBlockNum(){ return breakedBlockNum_; }

  uint32_t GetGrowInterval() { return growInterval_; }
    void SetGrowInterval(uint32_t interval) { growInterval_ = interval; }
    uint32_t GetNumGrowingBlocks() { return numGrowingBlocks_; }
    void SetNumGrowingBlocks(uint32_t numGrowingBlock) { numGrowingBlocks_ = numGrowingBlock; }
    void SetPlayer(Player* player) { player_ = player; }

    // mainで呼び出す
    // BGMのピッチを上げるため
    bool IsDangerousHeight() const { return isDangerousHeight_; }

    int32_t GetHeightestIndex();
    int32_t GetHeightestIndex(uint32_t xIndex);
    uint32_t GetLeatestIndex();

    uint32_t GetLeatestIndex(uint32_t exclusion);

private:
    Player* player_ = nullptr;

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
    void DrawScoreNum(uint32_t num, Vector2 centerPos, Vector2 size, Vector2 textureSize, TextureHandle textureHandle);
    void DrawGrow();

    void HarryEffect();
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
  	TextureHandle dangerousBlockTextureHandle_; // 危険なブロックに表示する
    Vector2 deadLinePosition_;
    Vector2 deadLineSize_;
    float deadLineAnimationFront_;
    float deadLineAnimationBack_;
    float deadLineColorH_;

    // ゲームオーバー
    const uint32_t kDeathLine_ = 10;
    uint32_t blockBleakAnimationCount_;
    int32_t heightCount_;

   
    // マップチップの位置を保存
    std::vector<std::unique_ptr<GameOver>> gameOverBlocks_;
    Vector2 gameOverPosition_;
    Vector2 gameOverPositionStart_;
    Vector2 gameOverPositionEnd_;
    uint32_t dropTextCount_;
    uint32_t gameOverBlockCount_;

    //　ゲームクリア
    uint32_t clearLine_;
    bool isVanish_;
    uint32_t breakedBlockNum_;
    uint32_t combedStepNum_;
    uint32_t combedHrizonNum_;

    //score
    float scoreT;
    // テクスチャハンドル
    TextureHandle numTextureHandle_;
    TextureHandle scoreTextureHandle_;

    size_t blockDownHandle_;
    size_t blockDown2Handle_;

    size_t scoreSoundHandle_;
    uint32_t scoreFrame_;

    bool isBlockBreaking_;
    bool isTextDropping_;
    bool isInGameOver_;
    bool isGameOver_;
    bool isDangerousHeight_;
};