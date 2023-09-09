#include "Player.h"

#include "TOMATOsEngine.h"

#include "Field.h"
#include "Particle/ParticleManager.h"
void Player::Initialize() {
    size_ = { 30.0f, 60.0f };
    velocity_ = Vector2::zero;

}

void Player::Update() {

   
    move();

    particleManager_->GetFollow()->Create(position_,Vector4(1.0f,1.0f,1.0f,1.0f), static_cast<uint32_t>(Follow::Texture::kStar));
    //particleManager_->GetYenLetter()->Create(position_,Vector4(1.0f,1.0f,1.0f,1.0f),static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
    //particleManager_->GetYenLetter()->Create(position_,Vector4(1.0f,1.0f,1.0f,1.0f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1),false);
}

void Player::move()
{

#pragma region 移動
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

#pragma endregion 

#pragma region 当たり判定

    float top = tempPosition.y + size_.x * 0.5f;
    float bottom = tempPosition.y - size_.y * 0.5f;
    float left = tempPosition.x - size_.x * 0.5f;
    float right = tempPosition.x + size_.x * 0.5f;

    /*float preTop = position_.y + size_.x * 0.5f;*/
    float preBottom = position_.y - size_.y * 0.5f;
    /*float preLeft = position_.x - size_.x * 0.5f;
    float preRight = position_.x + size_.x * 0.5f;*/

    uint32_t blockTop = field_->CalcBlockIndexY(top);
    uint32_t blockBottom = field_->CalcBlockIndexY(bottom);
    uint32_t blockLeft = field_->CalcBlockIndexX(left);
    uint32_t blockRight = field_->CalcBlockIndexX(right);

    /* uint32_t preBlockTop = field_->CalcBlockIndexY(preTop);*/
    uint32_t preBlockBottom = field_->CalcBlockIndexY(preBottom);
    /*uint32_t preBlockLeft = field_->CalcBlockIndexX(preLeft);
    uint32_t preBlockRight = field_->CalcBlockIndexX(preRight);*/

    // 壁ぞり処理

    // 左のポイントふたつがブロックだった場合
    if (field_->GetBlock(blockLeft, blockTop) == Field::Normal &&
        field_->GetBlock(blockLeft, blockBottom) == Field::Normal) {
        tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
    }

    // 右のポイントふたつがブロックだった場合
    if (field_->GetBlock(blockRight, blockTop) == Field::Normal &&
        field_->GetBlock(blockRight, blockBottom) == Field::Normal) {
        tempPosition.x = blockRight * Field::kBlockSize - size_.x * 0.5f - 0.1f;
    }

    top = tempPosition.y + size_.x * 0.5f;
    bottom = tempPosition.y - size_.y * 0.5f;
    left = tempPosition.x - size_.x * 0.5f;
    right = tempPosition.x + size_.x * 0.5f;

    blockTop = field_->CalcBlockIndexY(top);
    blockBottom = field_->CalcBlockIndexY(bottom);
    blockLeft = field_->CalcBlockIndexX(left);
    blockRight = field_->CalcBlockIndexX(right);


    // 下のポイントふたつがブロックじゃなかった場合
    if (!(field_->GetBlock(blockLeft, blockBottom) == Field::Normal &&
        field_->GetBlock(blockRight, blockBottom) == Field::Normal)) {
        //左下だけ当たった時
        if (field_->GetBlock(blockLeft, blockBottom) == Field::Normal) {
            if (preBlockBottom <= blockBottom) {
                tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
            }
        }

        //右下だけ当たった時
        if (field_->GetBlock(blockRight, blockBottom) == Field::Normal) {
            if (preBlockBottom <= blockBottom) {
                tempPosition.x = blockRight * Field::kBlockSize - size_.x * 0.5f - 0.1f;
            }
        }
    }

    top = tempPosition.y + size_.x * 0.5f;
    bottom = tempPosition.y - size_.y * 0.5f;
    left = tempPosition.x - size_.x * 0.5f;
    right = tempPosition.x + size_.x * 0.5f;

    blockTop = field_->CalcBlockIndexY(top);
    blockBottom = field_->CalcBlockIndexY(bottom);
    blockLeft = field_->CalcBlockIndexX(left);
    blockRight = field_->CalcBlockIndexX(right);


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

#pragma endregion

    //仮ポス代入
    position_ = tempPosition;
}

void Player::Draw() {
    Vector2 rectMinPos = position_ - size_ * 0.5f;
    Vector2 rectMaxPos = position_ + size_ * 0.5f;
    TOMATOsEngine::DrawRect(rectMinPos, rectMaxPos, 0x883333FF);
}

void Player::Bounce() {
}

