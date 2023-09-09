#pragma once

#include "Math/MathUtils.h"

class Field;
class ParticleManager;

class Player {
public:
    
    void Initialize();
    void Update();
    void Draw();

    void SetPosition(const Vector2& position) { position_ = position; }
    void SetField(Field* field) { field_ = field; }
    void SetParticleManager(ParticleManager* particleManager) { particleManager_ = particleManager; }
    
private:
    void Bounce();

    // 真ん中
    Vector2 position_;
    Vector2 size_;

    Vector2 velocity_;

    Field* field_;
    ParticleManager* particleManager_;
};