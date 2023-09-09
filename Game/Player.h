#pragma once

#include "Math/MathUtils.h"

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
    void ParticleUpdate(uint32_t x, uint32_t y);
    // 真ん中
    Vector2 position_;
    Vector2 size_;

    Vector2 velocity_;
    // コンボ
    const uint32_t kCombo_ = 3;
    // 階段
    uint32_t preStep_;
    uint32_t step_;
    uint32_t stepCount_;
    // 平行
    uint32_t preSameHeight_;
    uint32_t sameHeight_;
    uint32_t sameHeightCount_;


    Field* field_;
    ParticleManager* particleManager_;
};