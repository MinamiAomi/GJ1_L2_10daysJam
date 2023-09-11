#include "Player.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Field.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"

void Player::Initialize() {
    size_ = { 30.0f, 60.0f };
    velocity_ = Vector2::zero;
    // コンボ
    comboTextureHandle_.at(0) = TOMATOsEngine::LoadTexture("Resources/x1.png");
    comboTextureHandle_.at(1) = TOMATOsEngine::LoadTexture("Resources/x2.png");
    comboTextureHandle_.at(2) = TOMATOsEngine::LoadTexture("Resources/x3.png");
    comboPosition_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,float(TOMATOsEngine::kMonitorHeight) * 0.5f };
    comboSize_ = { 150.0f,150.0f };
    comboColorH_ = 0.0f;
    comboDrawCount_ = 0;
    comboDrawAngle_ = 0.0f;
    comboDrawSize_ = { 0.0f ,0.0f };
    // 階段;
    preStep_ = 0;
    step_ = 0;
    stepCount_ = -1;
    stepColorH_ = 307.0f / 360.0f;
    // 平行
    preSameHeight_ = 0;
    sameHeight_ = 0;
    sameHeightCount_ = -1;
    textureHandle_ = TOMATOsEngine::LoadTexture("Resources/player.png");
    sameHeightColorH_ = 63.0f / 360.0f;

    preHeight_ = -1;
    nowHeight_ = -1;

    h_ = 0.0f;
    bonusColor_ = { 0.0f,0.0f,0.0f,0.0f };

    animationFrame = 0;
    continueTextureNum = 3;
    break_ = false;

    comboSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/combo.wav");
}

void Player::Update() {

    move();

    SetBlockColor(nowHeight_);

    // コンボ数によってエフェクト変化
    if (stepCount_ == 1 || sameHeightCount_ == 1) {
        particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kPlayer));
        particleManager_->GetYenLetter()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 0.8f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
        particleManager_->GetYenLetter()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 0.8f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1), false);
    }
    else if (stepCount_ == 2 || sameHeightCount_ == 2) {
        particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kPlayer));
        particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kStar));
        particleManager_->GetYenLetter()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
        particleManager_->GetYenLetter()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1), false);
    }
    else if (stepCount_ == 0 || sameHeightCount_ == 0) {
        particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 0.5f), static_cast<uint32_t>(Follow::Texture::kPlayer));
    }

    ImGui::Begin("Player");
    ImGui::Text("preStep_:%d", preStep_);
    ImGui::Text("step_:%d", step_);
    ImGui::Text("stepCount_:%d", stepCount_);
    ImGui::Text("preSameHeight_:%d", preSameHeight_);
    ImGui::Text("sameHeight_:%d", sameHeight_);
    ImGui::Text("sameHeightCount_:%d", sameHeightCount_);
    ImGui::SliderFloat("position_X", &comboPosition_.x, 0.0f, 640.0f);
    ImGui::SliderFloat("position_Y", &comboPosition_.y, 0.0f, 480.0f);
    float size = comboSize_.x;
    ImGui::SliderFloat("size", &size, 0.0f, 300.0f);
    comboSize_ = { size,size };
    ImGui::End();

}

void Player::move() {

    const float kJumpPower = 20.0f;

#pragma region 移動
    velocity_.x = 0.0f;
    if (TOMATOsEngine::IsKeyPressed(DIK_D)) {
        velocity_.x += 3.5f;
    }
    if (TOMATOsEngine::IsKeyPressed(DIK_A)) {
        velocity_.x -= 3.5f;
    }

    velocity_.y -= 1.0f;
    velocity_.y = std::clamp(velocity_.y, -20.0f, 20.0f);

    // 仮移動
    Vector2 tempPosition = position_ + velocity_;
    tempPosition.x = std::clamp(tempPosition.x, size_.x * 0.5f, field_->GetSize().x - size_.x * 0.5f - 0.1f);

#pragma endregion 

#pragma region 当たり判定

    float top = tempPosition.y + size_.x * 0.5f;
    float mid = tempPosition.y;
    float bottom = tempPosition.y - size_.y * 0.5f;
    float left = tempPosition.x - size_.x * 0.5f;
    float right = tempPosition.x + size_.x * 0.5f;



    /*float preTop = position_.y + size_.x * 0.5f;*/
    float preBottom = position_.y - size_.y * 0.5f;
    /*float preLeft = position_.x - size_.x * 0.5f;
    float preRight = position_.x + size_.x * 0.5f;*/

    uint32_t blockTop = field_->CalcBlockIndexY(top);
    uint32_t blockMid = field_->CalcBlockIndexY(mid);
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
    mid = tempPosition.y;
    left = tempPosition.x - size_.x * 0.5f;
    right = tempPosition.x + size_.x * 0.5f;

    blockTop = field_->CalcBlockIndexY(top);
    blockMid = field_->CalcBlockIndexY(mid);
    blockBottom = field_->CalcBlockIndexY(bottom);
    blockLeft = field_->CalcBlockIndexX(left);
    blockRight = field_->CalcBlockIndexX(right);

    // 左のポイント(真ん中)ふたつがブロックだった場合
    if (field_->GetBlock(blockLeft, blockMid) == Field::Normal &&
        field_->GetBlock(blockLeft, blockBottom) == Field::Normal) {
        tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
    }

    // 右のポイント(真ん中)ふたつがブロックだった場合
    if (field_->GetBlock(blockRight, blockMid) == Field::Normal &&
        field_->GetBlock(blockRight, blockBottom) == Field::Normal) {
        tempPosition.x = blockRight * Field::kBlockSize - size_.x * 0.5f - 0.1f;
    }

    top = tempPosition.y + size_.x * 0.5f;
    bottom = tempPosition.y - size_.y * 0.5f;
    mid = tempPosition.y;
    left = tempPosition.x - size_.x * 0.5f;
    right = tempPosition.x + size_.x * 0.5f;

    blockTop = field_->CalcBlockIndexY(top);
    blockMid = field_->CalcBlockIndexY(mid);
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
                // 高さ更新
                preHeight_ = nowHeight_;
                velocity_.y = kJumpPower;
                animationFrame = (continueTextureNum - 1) * kAnimationSwitchNum;
                float blockTopPosition = 0.0f;



                if (bottom > 0.0f && bottom <= field_->GetSize().y) {
                    blockTopPosition = field_->GetBlockTop(blockBottom);
                }
                if (blockLeftBottomType != Field::None &&
                    field_->IsInField(blockLeft, blockBottom)) {
                    // ブロック破壊
                    if (stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
                        field_->BreakBlockHorizon(blockLeft, blockBottom);
                        isComboed_ = true;
                        // 高さ更新
                        nowHeight_ = blockBottom;
                        // コンボカウントリセット
                        stepCount_ = -1;
                        sameHeightCount_ = -1;
                        // リセットした後にすぐComboUpdateに入らないように
                        break_ = true;
                        // パーティクル
                        for (size_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
                            if (field_->GetBlock(static_cast<uint32_t>(x), blockBottom) == Field::Frash) {
                                CreateUpdate(static_cast<uint32_t>(x), blockBottom);
                            }
                        }
                    }
                    else {
                        field_->BreakBlock(blockLeft, blockBottom);
                        // 高さ更新
                        nowHeight_ = blockBottom;
                        // パーティクル
                        CreateUpdate(blockLeft, blockBottom);
                    }
                }
                if (blockRightBottomType != Field::None &&
                    field_->IsInField(blockRight, blockBottom)) {
                    // ブロック破壊
                    if (stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
                        field_->BreakBlockHorizon(blockRight, blockBottom);
                        // 高さ更新
                        nowHeight_ = blockBottom;
                        // コンボカウントリセット
                        stepCount_ = -1;
                        sameHeightCount_ = -1;
                        // リセットした後にすぐComboUpdateに入らないように
                        break_ = true;
                        // パーティクル
                        for (size_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
                            if (field_->GetBlock(static_cast<uint32_t>(x), blockBottom) == Field::Frash) {
                                CreateUpdate(static_cast<uint32_t>(x), blockBottom);
                            }
                        }
                    }
                    else {
                        field_->BreakBlock(blockRight, blockBottom);
                        // 高さ更新
                        nowHeight_ = blockBottom;
                        // パーティクル
                        CreateUpdate(blockRight, blockBottom);
                    }
                }
                tempPosition.y += blockTopPosition - bottom;
                // コンボアップデート
                ComboUpdate(bottom, blockLeft, blockBottom);
                // 地面に着いたら
                if (bottom <= 0.0f) {
                    nowHeight_ = -1;
                }
                SetBlockParticleColor(nowHeight_);
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
    animationFrame--;

    if (animationFrame > 0) {
        Vector2 texBase = { 0.0f,0.0f };
        texBase.x = (continueTextureNum - 1 - (animationFrame / kAnimationSwitchNum)) * 30.0f;
        TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, texBase, Vector2(30.0f, 60.0f), textureHandle_, 0xFFFFFFFF);
    }
    else {
        TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, {}, Vector2(30.0f, 60.0f), textureHandle_, 0xFFFFFFFF);
    }

    //TOMATOsEngine::DrawRect(rectMinPos, rectMaxPos, 0x883333FF);

    // コンボ数描画
    ComboDraw();
}

void Player::CreateUpdate(uint32_t x, uint32_t y) {
    // パーティクル
    particleManager_->GetSplash()->Create(
        Vector2(
            static_cast<float>(x * Field::kBlockSize),
            static_cast<float>(y * Field::kBlockSize)
        ),
        Vector4(1.0f, 1.0f, 1.0f, 1.0f),
        static_cast<uint32_t>(Pop::Texture::kWhite1x1),
        10);
    particleManager_->GetPop()->Create(
        Vector2(
            static_cast<float>(x * Field::kBlockSize),
            static_cast<float>(y * Field::kBlockSize)
        ),
        Vector4(1.0f, 1.0f, 1.0f, 1.0f),
        static_cast<uint32_t>(Pop::Texture::kBlock),
        10);
}

void Player::ComboUpdate(float  floor, uint32_t blockIndexX, uint32_t blockIndexY) {
    uint32_t x = blockIndexX;
    x = 0;
    // 地面に着いたらコンボカウントリセット
    if (!break_) {
        if (floor > 0) {
#pragma region 階段
            step_ = blockIndexY;
            if (step_ - 1 == preStep_) {
                stepCount_++;
                comboDrawCount_ = 0;

                if (stepCount_ >= 1) {
                    size_t playHandle = TOMATOsEngine::PlayAudio(comboSoundHandle_);
                    TOMATOsEngine::SetPitch(playHandle, 1.0f + stepCount_ * 0.1f);
                }
            }
            else {
                stepCount_ = 0;
                comboDrawCount_ = 0;
            }
            preStep_ = step_;
#pragma endregion
#pragma region 平行
            sameHeight_ = blockIndexY;
            if (sameHeight_ == preSameHeight_) {
                sameHeightCount_++;
             
                if (sameHeightCount_ >= 1) {
                    size_t playHandle = TOMATOsEngine::PlayAudio(comboSoundHandle_);
                    TOMATOsEngine::SetPitch(playHandle, 1.0f + sameHeightCount_ * 0.1f);
                }
            }
            else {
                sameHeightCount_ = 0;
                comboDrawCount_ = 0;
            }
            preSameHeight_ = sameHeight_;
        }
        else {
            stepCount_ = -1;
            sameHeightCount_ = -1;
        }
    }
    else {
        break_ = false;
    }
}

void Player::ComboDraw() {
    float tex1 = std::clamp(static_cast<float>(sameHeightCount_), -1.0f, 2.0f);
    float tex2 = std::clamp(static_cast<float>(stepCount_), -1.0f, 2.0f);
    if (stepCount_ > sameHeightCount_) {
        comboColorH_ = stepColorH_;
    }
    else {
        comboColorH_ = sameHeightColorH_;
    }
    float tex = std::max(tex1, tex2);
    if (tex >= 1) {
        Random::RandomNumberGenerator rnd{};
        const uint32_t comboTime = 20;
        Vector2 position{};
        float distance = 0.0f;
        float s = 0.0f;
        float v = 0.0f;
        float SizeMax = 0.0f;
        float AngleMax = 0.0f;
        comboDrawCount_++;
        float t = std::clamp(float(comboDrawCount_) / float(comboTime), 0.0f, 1.0f);
        if (tex == 0) {
            distance = 2.0f;
            s = kCombo1S_;
            v = kCombo1V_;
            SizeMax = 150.0f;
            AngleMax = 360.0f;
        }
        else if (tex == 1) {
            distance = 5.0f;
            s = kCombo2S_;
            v = kCombo2V_;
            SizeMax = 200.0f;
            AngleMax = 720.0f;
        }
        else {
            distance = 8.0f;
            comboColorH_ = h_;
            s = kCombo3S_;
            v = kCombo3V_;
            SizeMax = 300.0f;
            AngleMax = 1080.0f;
        }
        position.x = comboPosition_.x + rnd.NextFloatRange(-distance, distance);
        position.y = comboPosition_.y + rnd.NextFloatRange(-distance, distance);
        comboSize_.x = Math::Lerp(t, 0.0f, SizeMax);
        comboSize_.x = Math::Lerp(t, 0.0f, SizeMax);
        comboDrawAngle_ = Math::Lerp(t, 0.0f, AngleMax);
        TOMATOsEngine::DrawSpriteRectAngle(position, comboSize_, Vector2(0.5f, 0.5f), comboDrawAngle_ * Math::ToRadian, {}, Vector2(64.0f, 64.0f), comboTextureHandle_.at(static_cast<uint32_t>(tex)), Color::HSVA(comboColorH_, s, v));
    }
}

void Player::SetBlockColor(int32_t blockIndexY) {
    // 色
    field_->ColorClearBlock();
    if (blockIndexY != -1) {
        for (uint32_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
            // コンボ達成しているか
            if (stepCount_ < kCombo_ && sameHeightCount_ < kCombo_) {
                // 階段
                if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 2)) == Field::None && field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 1)) == Field::Normal) {
                    if (stepCount_ == 0) {
                        // 色
                        field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY + 1), Color::HSVA(stepColorH_, kCombo1S_, kCombo1V_));
                    }
                    else {
                        // 色
                        field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY + 1), Color::HSVA(stepColorH_, kCombo2S_, kCombo2V_));
                    }
                }
                // 平行
                if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 1)) == Field::None && field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY)) == Field::Normal) {
                    if (sameHeightCount_ == 0) {
                        // 色
                        field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo1S_, kCombo1V_));
                    }
                    else {
                        // 色
                        field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo2S_, kCombo2V_));
                    }
                }
            }
            else {
                for (uint32_t y = 0; y < Field::kNumVerticalBlocks; y++) {
                    if (field_->GetBlock(x, y) == Field::Normal) {
                        // 色
                        const float kAddH = 0.0008f;
                        h_ += kAddH;
                        if (h_ >= 1.0f) {
                            h_ = 0.0f;
                        }

                        field_->SetColorBlock(x, y, Color::HSVA(h_, kCombo3S_, kCombo3V_));
                    }
                }
            }
        }
    }
}

void Player::SetBlockParticleColor(int32_t blockIndexY) {
    if (blockIndexY != -1) {
        if (!isComboed_) {
            for (uint32_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
                // コンボ達成しているか
                if (stepCount_ < kCombo_ && sameHeightCount_ < kCombo_) {

                    // 階段
                    if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 2)) == Field::None && field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 1)) == Field::Normal) {
                        if (stepCount_ == 0) {
                            // 色
                            const float kS = 1.0f;
                            const float kV = 0.3f;
                            particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float((blockIndexY + 1) * Field::kBlockSize) + (Field::kBlockSize / 2)), Color::HSVA(stepColorH_, kS, kV), static_cast<uint32_t>(Circle::Texture::kSquare));
                        }
                        else {
                            // 色
                            const float kS = 1.0f;
                            const float kV = 1.0f;
                            particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float((blockIndexY + 1) * Field::kBlockSize) + (Field::kBlockSize / 2)), Color::HSVA(stepColorH_, kS, kV), static_cast<uint32_t>(Circle::Texture::kSquare));
                        }
                    }
                    // 平行
                    if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 1)) == Field::None && field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY)) == Field::Normal) {
                        if (sameHeightCount_ == 0) {
                            // 色
                            const float kS = 1.0f;
                            const float kV = 0.3f;
                            particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float(blockIndexY * Field::kBlockSize) + (Field::kBlockSize / 2)), Color::HSVA(sameHeightColorH_, kS, kV), static_cast<uint32_t>(Circle::Texture::kSquare));
                        }
                        else {
                            // 色
                            const float kS = 1.0f;
                            const float kV = 1.0f;
                            particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float(blockIndexY * Field::kBlockSize) + (Field::kBlockSize / 2)), Color::HSVA(sameHeightColorH_, kS, kV), static_cast<uint32_t>(Circle::Texture::kSquare));
                        }
                    }


                }
            }
        }
        else {
            isComboed_ = true;
        }
    }
}

