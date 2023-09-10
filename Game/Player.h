#pragma once

#include <array>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Field;
class ParticleManager;

class Player {
public:
    
    void Initialize();
    void Update();
    void move();
    void Draw();

    void SetPosition(const Vector2& position) { position_ = position; }
    void SetField(Field* field) { field_ = field; }
    void SetParticleManager(ParticleManager* particleManager) { particleManager_ = particleManager; }
    
private:
    static const int32_t kAnimationSwitchNum = 10;

    void ComboUpdate(float floor, uint32_t x, uint32_t y);
    void ComboDraw();
    void SetBlockColor(int32_t y);
    void SetBlockParticleColor(int32_t y);
    void CreateUpdate(uint32_t x, uint32_t y);
    // 真ん中
    Vector2 position_;
    Vector2 size_;

    Vector2 velocity_;
    // コンボ
    const int32_t kCombo_ = 2;
    std::array<TextureHandle, 3> comboTextureHandle_;
    Vector2 comboPosition_;
    Vector2 comboSize_;
    Vector4 comboColor_;
    bool break_;
    // 虹色
    float h_;
    Vector4 bonusColor_;
    // 階段
    uint32_t preStep_;
    uint32_t step_;
    int32_t stepCount_;
    float stepColorH_;
    // 平行
    uint32_t preSameHeight_;
    uint32_t sameHeight_;
    int32_t sameHeightCount_;
    float sameHeightColorH_;

    int32_t preHeight_;
    int32_t nowHeight_;

    Field* field_;
    ParticleManager* particleManager_;
    // テクスチャハンドル
    TextureHandle textureHandle_;
    //animation
    int32_t animationFrame;
    uint32_t continueTextureNum;
};