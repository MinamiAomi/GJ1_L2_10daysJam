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
	comboPosition_ = { 487.0f,367.0f };
	comboSize_ = { 150.0f,150.0f };
	comboColor_ = { 1.0f,1.0f,1.0f,1.0f };
	// 階段;
	preStep_ = 0;
	step_ = 0;
	stepCount_ = 0;
	stepColor_ = Vector4(0.627f, 0.152f, 0.690f, 1.0f);
	// 平行
	preSameHeight_ = 0;
	sameHeight_ = 0;
	sameHeightCount_ = 0;
  textureHandle_ = TOMATOsEngine::LoadTexture("Resources/player.png");
	sameHeightColor_ = Vector4(0.662f, 0.690f, 0.156f, 1.0f);
}

void Player::Update() {

	move();

	particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kPlayer));
	//particleManager_->GetYenLetter()->Create(position_,Vector4(1.0f,1.0f,1.0f,1.0f),static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
	//particleManager_->GetYenLetter()->Create(position_,Vector4(1.0f,1.0f,1.0f,1.0f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1),false);

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
	tempPosition.x = std::clamp(tempPosition.x, size_.x * 0.5f, field_->GetSize().x - size_.x * 0.5f - 0.1f);

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
				// コンボアップデート
				ComboUpdate(bottom, blockLeft, blockBottom);
				if (blockLeftBottomType != Field::None &&
					field_->IsInField(blockLeft, blockBottom)) {
					// ブロック破壊
					if (stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
						field_->BreakBlockHorizon(blockLeft, blockBottom);
						stepCount_ = 0;
						sameHeightCount_ = 0;
						SetBlockColor(blockLeft, blockBottom);
					}
					else {
						field_->BreakBlock(blockLeft, blockBottom);
						SetBlockColor(blockLeft, blockBottom);

					}
					// パーティクル
					ParticleUpdate(blockLeft, blockBottom);
				}
				if (blockRightBottomType != Field::None &&
					field_->IsInField(blockRight, blockBottom)) {
					// ブロック破壊
					if (stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
						field_->BreakBlockHorizon(blockRight, blockBottom);
						stepCount_ = 0;
						sameHeightCount_ = 0;
						SetBlockColor(blockRight, blockBottom);
					}
					else {
						field_->BreakBlock(blockRight, blockBottom);
						SetBlockColor(blockRight, blockBottom);
					}
					// パーティクル
					ParticleUpdate(blockRight, blockBottom);
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

    TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, {}, Vector2(30.0f, 60.0f), textureHandle_, 0xFFFFFFFF);
	//TOMATOsEngine::DrawRect(rectMinPos, rectMaxPos, 0x883333FF);

  // コンボ数描画
  float tex1 = std::clamp(static_cast<float>(sameHeightCount_), 0.0f, 2.0f);
	float tex2 = std::clamp(static_cast<float>(stepCount_), 0.0f, 2.0f);
	float tex = std::max(tex1, tex2);
	if (tex >= 1) {
		float distance = 0.0f;
		float angle_Origin = 0.0f;
		if (tex == 1) {
			distance = 2.0f;
			angle_Origin = 3.0f;
		}
		else {
			distance = 5.0f;
			angle_Origin = 8.0f;
		}
		Random::RandomNumberGenerator rnd{};
		Vector2 position{};
		position.x = comboPosition_.x + rnd.NextFloatRange(-distance, distance);
		position.y = comboPosition_.y + rnd.NextFloatRange(-distance, distance);
		float angle = rnd.NextFloatRange(-angle_Origin, angle_Origin);
		TOMATOsEngine::DrawSpriteRectAngle(position, comboSize_, Vector2(0.5f, 0.5f), angle * Math::ToRadian, {}, Vector2(64.0f, 64.0f), comboTextureHandle_.at(static_cast<uint32_t>(tex)), Color(comboColor_));
	}

}

void Player::Bounce() {}

void Player::ParticleUpdate(uint32_t x, uint32_t y) {
	// パーティクル
	particleManager_->GetSplash()->Create(
		Vector2(
			static_cast<float>(x * Field::kBlockSize),
			static_cast<float>(y * Field::kBlockSize)
		),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		static_cast<uint32_t>(Pop::Texture::kWhite1x1),
		20);
	particleManager_->GetPop()->Create(
		Vector2(
			static_cast<float>(x * Field::kBlockSize),
			static_cast<float>(y * Field::kBlockSize)
		),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		static_cast<uint32_t>(Pop::Texture::kBlock),
		20);
}

void Player::ComboUpdate(float  floor, uint32_t blockIndexX, uint32_t blockIndexY) {
	uint32_t x = blockIndexX;
	x = 0;
	// 地面に着いたらコンボカウントリセット
	if (floor > 0) {
#pragma region 階段
		step_ = blockIndexY;
		if (step_ - 1 == preStep_) {
			stepCount_++;
		}
		else {
			stepCount_ = 0;
		}
		preStep_ = step_;
#pragma endregion
#pragma region 平行
		sameHeight_ = blockIndexY;
		if (sameHeight_ == preSameHeight_) {
			sameHeightCount_++;
		}
		else {
			sameHeightCount_ = 0;
		}
		preSameHeight_ = sameHeight_;
	}
	else {
		stepCount_ = 0;
		sameHeightCount_ = 0;
	}
}

void Player::SetBlockColor(uint32_t blockIndexX, uint32_t blockIndexY) {
	blockIndexX = 0;
	// 色
	field_->ColorClearBlock();
	
	for (size_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
		// 階段
		if (field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 2)) == Field::None) {
			field_->SetColorBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 1), stepColor_);
		}
		// 平行
		if (field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 1)) == Field::None) {
			field_->SetColorBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY), sameHeightColor_);
		}
	}
}



