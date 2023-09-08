#include "Player.h"

#include "TOMATOsEngine.h"

#include "Field.h"

void Player::Initialize() {
    size_ = { 30.0f, 60.0f };
    velocity_ = Vector2::zero;

}

void Player::Update() {
    
    velocity_.y -= 1.0f;
    velocity_.y = std::clamp(velocity_.y, -10.0f, 10.0f);
    
    // 仮移動
    Vector2 tempPosition = position_ + velocity_;

    float bottom = tempPosition.y - size_.y * 0.5f;
    float left = tempPosition.x - size_.x * 0.5f;
    float right = tempPosition.x + size_.x * 0.5f;

    uint32_t blockBottom = field_->CalcBlockIndexY(bottom);
    uint32_t blockLeft = field_->CalcBlockIndexX(left);
    uint32_t blockRight = field_->CalcBlockIndexX(right);

    if (field_->GetBlock(blockLeft, blockBottom) != Field::None ||
        field_->GetBlock(blockRight, blockBottom) != Field::None) {
        velocity_.y *= -1.0f;
        float blockTopPosition = field_->GetBlockTop(blockBottom);
        

        tempPosition.y += blockTopPosition - bottom;
    }

    position_ = tempPosition;
}

void Player::Draw() {
    Vector2 rectMinPos = position_ - size_ * 0.5f;
    Vector2 rectMaxPos = position_ + size_ * 0.5f;
    TOMATOsEngine::DrawRect(rectMinPos, rectMaxPos, 0xFF333388);
}
