#include "Player.h"

#include "TOMATOsEngine.h"

#include "Field.h"

void Player::Initialize() {
    size_ = { 30.0f, 60.0f };
    velocity_ = Vector2::zero;

}

void Player::Update() {

    velocity_.x = 0.0f;
    if (TOMATOsEngine::IsKeyPressed(DIK_D)) {
        velocity_.x += 3.0f;
    }
    if (TOMATOsEngine::IsKeyPressed(DIK_A)) {
        velocity_.x -= 3.0f;
    }

    velocity_.y -= 1.0f;
    velocity_.y = std::clamp(velocity_.y, -20.0f, 20.0f);

    // 仮移動
    Vector2 tempPosition = position_ + velocity_;

    //float top = tempPosition.y + size_.x * 0.5f;
    float bottom = tempPosition.y - size_.y * 0.5f;
    float left = tempPosition.x - size_.x * 0.5f;
    float right = tempPosition.x + size_.x * 0.5f;

    //uint32_t blockTop = field_->CalcBlockIndexY(top);
    uint32_t blockBottom = field_->CalcBlockIndexY(bottom);
    uint32_t blockLeft = field_->CalcBlockIndexX(left);
    uint32_t blockRight = field_->CalcBlockIndexX(right);

    //// 壁ぞり処理
    //// 右
    //// フィールド内
    //if (right <= field_->GetSize().x) {
    //    // 縦幅がチップサイズより大きい可能性があるので
    //    // 下のチップ番号と上のチップ番号から乗っている可能性のある範囲を取得
    //    uint32_t indexRange = std::abs(int(blockTop) - int(blockBottom));
    //    for (uint32_t i = 0; i < indexRange; ++i) {
    //        auto blockRightType = field_->GetBlock(blockRight, blockBottom + i);
    //        if (blockRightType != Field::None) {
    //            // 衝突ブロックの左側を取得
    //            float blockRightPosition = field_->GetBlockLeft(blockRight);
    //            // ずらす(1.0f)は次でめり込まないように
    //            tempPosition.x += blockRightPosition - right - 1.0f;
    //            velocity_.x = 0.0f;
    //            break;
    //        }

    //    }
    //}
    //else {
    //    tempPosition.x += field_->GetSize().x - right - 1.0f;
    //    velocity_.x = 0.0f;
    //}

    {
        auto blockLeftBottomType = field_->GetBlock(blockLeft, blockBottom);
        auto blockRightBottomType = field_->GetBlock(blockRight, blockBottom);
        
        if (bottom < field_->GetSize().y) {
            // 跳ねる処理
            if (blockLeftBottomType != Field::None ||
                blockRightBottomType != Field::None ||
                bottom <= 0.0f) {
                velocity_.y = 20.0f;
                float blockTopPosition = 0.0f;
                if (bottom > 0.0f && bottom <= field_->GetSize().y) {
                    blockTopPosition = field_->GetBlockTop(blockBottom);
                }

                if (blockLeftBottomType != Field::None &&
                    field_->IsInField(blockLeft, blockBottom)) {
                    field_->BreakBlock(blockLeft, blockBottom);
                }
                if (blockRightBottomType != Field::None &&
                    field_->IsInField(blockRight, blockBottom)) {
                    field_->BreakBlock(blockRight, blockBottom);
                }
                tempPosition.y += blockTopPosition - bottom;
            }
        }
    }

    position_ = tempPosition;
}

void Player::Draw() {
    Vector2 rectMinPos = position_ - size_ * 0.5f;
    Vector2 rectMaxPos = position_ + size_ * 0.5f;
    TOMATOsEngine::DrawRect(rectMinPos, rectMaxPos, 0x883333FF);
}

void Player::Bounce() {
}
