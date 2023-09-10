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
    void Bounce();

    void ComboUpdate(float floor, uint32_t x, uint32_t y);
    void SetBlockColor(uint32_t x, uint32_t y);
    void ParticleUpdate(uint32_t x, uint32_t y);
    // 真ん中
    Vector2 position_;
    Vector2 size_;

    Vector2 velocity_;
    // コンボ
    const uint32_t kCombo_ = 3;
    std::array<TextureHandle, 3> comboTextureHandle_;
    Vector2 comboPosition_;
    Vector2 comboSize_;
    Vector4 comboColor_;

    // 階段
    uint32_t preStep_;
    uint32_t step_;
    uint32_t stepCount_;
    Vector4 stepColor_;
    // 平行
    uint32_t preSameHeight_;
    uint32_t sameHeight_;
    uint32_t sameHeightCount_;
    Vector4 sameHeightColor_;

    Field* field_;
    ParticleManager* particleManager_;
    // テクスチャハンドル
    TextureHandle textureHandle_;
};