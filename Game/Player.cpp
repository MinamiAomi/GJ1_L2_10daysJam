#include "Player.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Field.h"
#include "FeverManager.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"
#include "GameTime.h"
#include "Easing.h"

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
	isComboed_ = false;
	// 階段;
	preStep_ = 0;
	step_ = 0;
	stepCount_ = -1;
	stepColorH_ = 63.0f / 360.0f;
	// 平行
	// 0だと怖い
	preSameHeightX_ = 20;
	sameHeightX_ = 20;
	isDifferentX_ = false;
	preSameHeightY_ = 0;
	sameHeightY_ = 0;
	sameHeightCount_ = -1;
	textureHandle_ = TOMATOsEngine::LoadTexture("Resources/player.png");
	sameHeightColorH_ = 307.0f / 360.0f;
	sameHeightColorChangePositionRight_ = { 0.0f,0.0f };
	sameHeightColorChangePositionLeft_ = { 0.0f,0.0f };
	sameHeightColorChangeVelocity_ = { 0.0f,0.0f };
	sameHeightStart_ = false;
	for (auto& particle : sameHeightParticleFlag_) {
		particle = false;
	}

	preHeight_ = -1;
	nowHeight_ = -1;

	h_ = 0.0f;
	bonusColor_ = { 0.0f,0.0f,0.0f,0.0f };

	animationFrame_ = 0;
	continueTextureNum_ = 3;
	break_ = false;

	comboSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/combo.wav");
	groundJumpSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/groundJump.wav");

	gameOverVelocity_ = { 0.0f,0.0f };
	gameOverAngle_ = 0.0f;

	clearTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/clearPlayer.png");

	isSaveClearPos_ = false;
	gameClearRadian_ = 0.0f;
	gameClearT_ = 0.0f;
	isHorizontal_ = false;
	isEndGameClearEasing_ = false;
	gameClearMoveCoolTime_ = kGameClearMoveCoolTime_;
	nowWeight_ = 0;

	yosokuHandle_ = TOMATOsEngine::LoadTexture("Resources/yosoku.png");

	sameHeightTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/horizon.png");
	stepTextureHandle_ = TOMATOsEngine::LoadTexture("Resources/stairs.png");

	sameHeightInfo_.clear();
	stepInfo_.clear();
}

void Player::Update() {
	GameTime* gameTime = GameTime::GetInstance();

	if (!field_->GetIsInGameOver()) {
		if (!gameTime->GetIsFinish()) {
			move();

			SetBlockColor(nowHeight_);

			SkillUpdate();
			// コンボ数によってエフェクト変化
			if (stepCount_ == 1 || sameHeightCount_ == 1) {
				particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kPlayer));

				particleManager_->GetYenLetter()->Create(position_, Color::HSVA(comboColorH_, 1.0f, 0.5f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
				particleManager_->GetYenLetter()->Create(position_, Color::HSVA(comboColorH_, 1.0f, 0.5f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1), false);

			}
			else if (stepCount_ == 2 || sameHeightCount_ == 2) {
				particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kPlayer));
				particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 1.0f), static_cast<uint32_t>(Follow::Texture::kStar));

				particleManager_->GetYenLetter()->Create(position_, Color::HSVA(comboColorH_, 1.0f, 0.8f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1));
				particleManager_->GetYenLetter()->Create(position_, Color::HSVA(comboColorH_, 1.0f, 0.8f), static_cast<uint32_t>(YenLetter::Texture::kWhite1x1), false);
			}
			else if (stepCount_ == 0 || sameHeightCount_ == 0) {
				particleManager_->GetFollow()->Create(position_, Vector4(1.0f, 1.0f, 1.0f, 0.5f), static_cast<uint32_t>(Follow::Texture::kPlayer));
			}

		}
		else {

		}
	}
	else {
		GameOverUpdate();
	}

#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::Text("preStep_:%d", preStep_);
	ImGui::Text("step_:%d", step_);
	ImGui::Text("stepCount_:%d", stepCount_);
	/*ImGui::Text("preSameHeight_:%d", preSameHeight_);
	ImGui::Text("sameHeight_:%d", sameHeight_);*/
	ImGui::Text("sameHeightCount_:%d", sameHeightCount_);
	ImGui::SliderFloat("position_X", &comboPosition_.x, 0.0f, 640.0f);
	ImGui::SliderFloat("position_Y", &comboPosition_.y, 0.0f, 480.0f);
	float size = comboSize_.x;
	ImGui::SliderFloat("size", &size, 0.0f, 300.0f);
	comboSize_ = { size,size };
	ImGui::End();
#endif // _DEBUG

}

void Player::move() {

	const float kJumpPower = 20.0f;

#pragma region 移動
	velocity_.x = 0.0f;
	const auto& xInputState = TOMATOsEngine::GetGamePadState();

	if (TOMATOsEngine::IsKeyPressed(DIK_D) ||
		TOMATOsEngine::IsKeyPressed(DIK_RIGHT) ||
		xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
		xInputState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		velocity_.x += 3.5f;
	}
	if (TOMATOsEngine::IsKeyPressed(DIK_A) ||
		TOMATOsEngine::IsKeyPressed(DIK_LEFT) ||
		xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
		-xInputState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
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
	float midY = tempPosition.y + 10;
	float bottom = tempPosition.y - size_.y * 0.5f;
	float left = tempPosition.x - size_.x * 0.5f;
	float right = tempPosition.x + size_.x * 0.5f;

	bool isLeftHit = false;
	bool isRightHit = false;

	/*float preTop = position_.y + size_.x * 0.5f;*/
	float preBottom = position_.y - size_.y * 0.5f;
	/*float preLeft = position_.x - size_.x * 0.5f;
	float preRight = position_.x + size_.x * 0.5f;*/

	uint32_t blockTop = field_->CalcBlockIndexY(top);
	uint32_t blockMidY = field_->CalcBlockIndexY(midY);
	uint32_t blockBottom = field_->CalcBlockIndexY(bottom);
	uint32_t blockLeft = field_->CalcBlockIndexX(left);
	uint32_t blockRight = field_->CalcBlockIndexX(right);

	/* uint32_t preBlockTop = field_->CalcBlockIndexY(preTop);*/
	uint32_t preBlockBottom = field_->CalcBlockIndexY(preBottom);
	/*uint32_t preBlockLeft = field_->CalcBlockIndexX(preLeft);
	uint32_t preBlockRight = field_->CalcBlockIndexX(preRight);*/

	// 壁ぞり処理

	// 左のポイントふたつがブロックだった場合
	if (field_->GetBlock(blockLeft, blockTop) != Field::None &&
		field_->GetBlock(blockLeft, blockBottom) != Field::None) {
		tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
	}

	// 右のポイントふたつがブロックだった場合
	if (field_->GetBlock(blockRight, blockTop) != Field::None &&
		field_->GetBlock(blockRight, blockBottom) != Field::None) {
		tempPosition.x = blockRight * Field::kBlockSize - size_.x * 0.5f - 0.1f;
	}

	top = tempPosition.y + size_.x * 0.5f;
	bottom = tempPosition.y - size_.y * 0.5f;
	midY = tempPosition.y + 10;
	left = tempPosition.x - size_.x * 0.5f;
	right = tempPosition.x + size_.x * 0.5f;

	blockTop = field_->CalcBlockIndexY(top);
	blockMidY = field_->CalcBlockIndexY(midY);
	blockBottom = field_->CalcBlockIndexY(bottom);
	blockLeft = field_->CalcBlockIndexX(left);
	blockRight = field_->CalcBlockIndexX(right);

	// 左のポイント(真ん中)ふたつがブロックだった場合
	if (field_->GetBlock(blockLeft, blockMidY) != Field::None &&
		field_->GetBlock(blockLeft, blockBottom) != Field::None) {
		tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
	}

	// 右のポイント(真ん中)ふたつがブロックだった場合
	if (field_->GetBlock(blockRight, blockMidY) != Field::None &&
		field_->GetBlock(blockRight, blockBottom) != Field::None) {
		tempPosition.x = blockRight * Field::kBlockSize - size_.x * 0.5f - 0.1f;
	}

	top = tempPosition.y + size_.x * 0.5f;
	bottom = tempPosition.y - size_.y * 0.5f;
	midY = tempPosition.y + 10;
	left = tempPosition.x - size_.x * 0.5f;
	right = tempPosition.x + size_.x * 0.5f;

	blockTop = field_->CalcBlockIndexY(top);
	blockMidY = field_->CalcBlockIndexY(midY);
	blockBottom = field_->CalcBlockIndexY(bottom);
	blockLeft = field_->CalcBlockIndexX(left);
	blockRight = field_->CalcBlockIndexX(right);

	// 下のポイントふたつがブロックじゃなかった場合
	if (!(field_->GetBlock(blockLeft, blockBottom) != Field::None &&
		field_->GetBlock(blockRight, blockBottom) != Field::None)) {
		//左下だけ当たった時
		if (field_->GetBlock(blockLeft, blockBottom) != Field::None) {
			if (preBlockBottom <= blockBottom) {
				tempPosition.x = (blockLeft + 1) * Field::kBlockSize + size_.x * 0.5f + 0.1f;
			}
		}

		//右下だけ当たった時
		if (field_->GetBlock(blockRight, blockBottom) != Field::None) {
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

		uint32_t blockMidX = field_->CalcBlockIndexX(tempPosition.x);
		auto blockLeftBottomType = field_->GetBlock(blockLeft, blockBottom);
		auto blockRightBottomType = field_->GetBlock(blockRight, blockBottom);

		if (bottom < field_->GetSize().y) {
			// 跳ねる処理
			if (blockLeftBottomType == Field::Normal ||
				blockRightBottomType == Field::Normal ||
				bottom <= 0.0f) {
				// 高さ更新
				uint32_t nowWidth = 0;
				preHeight_ = nowHeight_;
				velocity_.y = kJumpPower;
				animationFrame_ = (continueTextureNum_ - 1) * kAnimationSwitchNum;
				float blockTopPosition = 0.0f;



				if (bottom > 0.0f && bottom <= field_->GetSize().y) {
					blockTopPosition = field_->GetBlockTop(blockBottom);
				}
				if (blockLeftBottomType == Field::Normal &&
					field_->IsInField(blockLeft, blockBottom)) {
					// ブロック破壊
					if (FeverManager::GetInstance()->GetIsFever() ||
						stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
						field_->BreakBlockHorizon(blockLeft, blockBottom, isHorizontal_);
						isComboed_ = false;
						// 高さ更新
						nowHeight_ = blockBottom;
						//nowWidth  = blockLeft;
						// コンボカウントリセット
						stepCount_ = -1;
						sameHeightCount_ = -1;
						// リセットした後にすぐComboUpdateに入らないように
						break_ = true;
						// パーティクル
						for (size_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
							if (field_->GetBlock(static_cast<uint32_t>(x), blockBottom) == Field::Frash) {
								CreateParticle(static_cast<uint32_t>(x), blockBottom);
							}
						}
					}
					else {
						isLeftHit = true;
						// 高さ更新
						nowHeight_ = blockBottom;
						//nowWidth = blockLeft;
						// パーティクル
						CreateParticle(blockLeft, blockBottom);
					}
				}
				if (blockRightBottomType == Field::Normal &&
					field_->IsInField(blockRight, blockBottom)) {
					// ブロック破壊
					if (FeverManager::GetInstance()->GetIsFever() ||
						stepCount_ >= kCombo_ || sameHeightCount_ >= kCombo_) {
						field_->BreakBlockHorizon(blockRight, blockBottom, isHorizontal_);
						isComboed_ = false;
						// 高さ更新
						nowHeight_ = blockBottom;
						//nowWidth = blockRight;
						// コンボカウントリセット
						stepCount_ = -1;
						sameHeightCount_ = -1;
						// リセットした後にすぐComboUpdateに入らないように
						break_ = true;
						// パーティクル
						for (size_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
							if (field_->GetBlock(static_cast<uint32_t>(x), blockBottom) == Field::Frash) {
								CreateParticle(static_cast<uint32_t>(x), blockBottom);
							}
						}
					}
					else {
						isRightHit = true;
						// 高さ更新
						nowHeight_ = blockBottom;
						//nowWidth = blockRight;
						// パーティクル
						CreateParticle(blockRight, blockBottom);
					}
				}
				if (isRightHit && isLeftHit) {
					field_->BreakBlock(blockMidX, blockBottom);
					nowWidth = blockMidX;
				}
				else if (isRightHit) {
					field_->BreakBlock(blockRight, blockBottom);
					nowWidth = blockRight;
				}
				else if (isLeftHit) {
					field_->BreakBlock(blockLeft, blockBottom);
					nowWidth = blockLeft;
				}


				tempPosition.y += blockTopPosition - bottom;
				// コンボアップデート
				ComboUpdate(bottom, nowWidth, blockBottom);
				// 地面に着いたら
				if (bottom <= 0.0f) {
					nowHeight_ = -1;
				}
				SetColorChange(tempPosition, nowHeight_);
				SetBlockParticleColor(nowHeight_);
			}
		}
	}

#pragma endregion

	//仮ポス代入
	position_ = tempPosition;

	issYosoku_ = false;

	uint32_t blockMid = field_->CalcBlockIndexX(position_.x);

	int rightHeightestIndex = field_->GetHeightestIndex(blockRight);
	int leftHeightestIndex = field_->GetHeightestIndex(blockLeft);

	if (rightHeightestIndex != -1 || leftHeightestIndex != -1) {
		issYosoku_ = true;
		if (rightHeightestIndex < leftHeightestIndex) {
			dropIndex_ = { float(blockLeft) ,float(leftHeightestIndex) };
		}
		else if (leftHeightestIndex < rightHeightestIndex) {
			dropIndex_ = { float(blockRight) ,float(rightHeightestIndex) };
		}
		else {
			dropIndex_ = { float(blockMid) ,float(rightHeightestIndex) };
		}
	}

}

void Player::Draw() {

	if (!field_->GetIsVanish()) {

		Vector2 rectMinPos = position_ - size_ * 0.5f;
		Vector2 rectMaxPos = position_ + size_ * 0.5f;
		animationFrame_--;

		if (animationFrame_ > 0) {
			Vector2 texBase = { 0.0f,0.0f };
			texBase.x = (continueTextureNum_ - 1 - (animationFrame_ / kAnimationSwitchNum)) * 30.0f;
			TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, texBase, Vector2(30.0f, 60.0f), textureHandle_, 0xFFFFFFFF);
		}
		else {
			TOMATOsEngine::DrawSpriteRect(rectMinPos, rectMaxPos, {}, Vector2(30.0f, 60.0f), textureHandle_, 0xFFFFFFFF);
		}

		//予測ボックス
		if (issYosoku_ && !field_->GetIsInGameOver()) {
			TOMATOsEngine::DrawSpriteRectCenter({ dropIndex_.x * Field::kBlockSize + Field::kBlockSize / 2.0f,dropIndex_.y * Field::kBlockSize + Field::kBlockSize / 2.0f }, { Field::kBlockSize,Field::kBlockSize }, { 0.0f,0.0f }, { 32.0f,32.0f }, yosokuHandle_, 0xFFFFFFFF);
		}
		if (!field_->GetIsInGameOver()) {
			SkillDraw();
		}
		// 円
		/*TOMATOsEngine::DrawCircle(sameHeightColorChangePositionRight_, 5.0f, 0x66666666);
		TOMATOsEngine::DrawCircle(sameHeightColorChangePositionLeft_, 5.0f, 0x66666666);*/
	}
	else {

		gameClearMoveCoolTime_--;
		if (gameClearMoveCoolTime_ <= 0) {
			gameClearRadian_ = Easing::easing(gameClearT_, 0.0f, (20.0f + 360.0f * 3) * Math::ToRadian, 0.005f, Easing::easeOutQuint, false);
			gameClearSize_ = Easing::easing(gameClearT_, Vector2{ 30.0f,60.0f }, Vector2{ 30.0f * 10.0f,60.0f * 10.0f }, 0.005f, Easing::easeOutQuint, false);
			gameClearPos_ = Easing::easing(gameClearT_, position_, Vector2{ TOMATOsEngine::kMonitorWidth / 4.0f - 20.0f, TOMATOsEngine::kMonitorHeight / 2.0f - 100.0f }, 0.01f, Easing::easeOutQuint);
		}
		else {
			gameClearRadian_ = Easing::easing(gameClearT_, 0.0f, (20.0f + 360.0f * 3) * Math::ToRadian, 0.005f, Easing::easeOutQuint, false);
			gameClearSize_ = Easing::easing(gameClearT_, Vector2{ 30.0f,60.0f }, Vector2{ 30.0f * 10.0f,60.0f * 10.0f }, 0.005f, Easing::easeOutQuint, false);
			gameClearPos_ = Easing::easing(gameClearT_, position_, Vector2{ TOMATOsEngine::kMonitorWidth / 4.0f - 20.0f, TOMATOsEngine::kMonitorHeight / 2.0f - 100.0f }, 0.01f, Easing::easeOutQuint, false);
		}


		if (gameClearT_ >= 1.0f) {
			isEndGameClearEasing_ = true;
		}
		TOMATOsEngine::DrawSpriteRectAngle(gameClearPos_, gameClearSize_, Vector2(0.5f, 0.5f), gameClearRadian_, {}, Vector2(270.0f, 540.0f), clearTextureHandle_, 0xFFFFFFFF);
	}
}

void Player::CreateParticle(uint32_t x, uint32_t y) {
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

void Player::SetColorChange(const Vector2& position, uint32_t nowHeight) {
	// 地面だったら入らない
	if (nowHeight != -1) {
		sameHeightColorChangePositionRight_ = position;
		sameHeightColorChangePositionLeft_ = position;
		// Yをプレイヤーの1ブロックしたの真ん中に設置
		sameHeightColorChangePositionRight_.y = float(uint32_t(sameHeightColorChangePositionRight_.y) / Field::kBlockSize) * float(Field::kBlockSize) - float(Field::kBlockSize) * 0.5f;
		sameHeightColorChangePositionLeft_.y = float(uint32_t(sameHeightColorChangePositionLeft_.y) / Field::kBlockSize) * float(Field::kBlockSize) - float(Field::kBlockSize) * 0.5f;
		sameHeightStart_ = true;
		for (auto& particle : sameHeightParticleFlag_) {
			particle = false;
		}
	}

}

void Player::GameOverUpdate() {
	/*if (position_.y > field_->GetGameOverPosition().y) {
		Vector2 move{};
		const float kSpeed = 15.0f;
		Random::RandomNumberGenerator rnd{};
		gameOverVelocity_.x = std::cos(rnd.NextFloatRange(-1.0f, 1.0f));
		gameOverVelocity_.y = std::sin(rnd.NextFloatRange(-1.0f, 1.0f));
		gameOverVelocity_.Normalized();
		gameOverVelocity_ *= kSpeed;
	}*/
}

void Player::GameClearUpdate() {

}

void Player::SkillUpdate() {
	if (!isSkillSprite_) {
		if (stepCount_ >= kCombo_) {
			if (!stepInfo_.empty()) {
				for (auto& step : stepInfo_) {
					step.position.y += skillSize_.y * 0.5f + 5.0f;
				}
			}
			if (!sameHeightInfo_.empty()) {
				for (auto& sameHeight : sameHeightInfo_) {
					sameHeight.position.y += skillSize_.y * 0.5f + 5.0f;
				}
			}
			SpriteInfo info{};
			info.position = {};
			info.color = { 1.0f,1.0f,1.0f,1.0f };
			info.time = 0.0f;
			info.isAlive = true;
			info.isAnimation = true;
			stepInfo_.emplace_back(info);
			isSkillSprite_ = true;
		}
		if (sameHeightCount_ >= kCombo_) {
			if (!stepInfo_.empty()) {
				for (auto& step : stepInfo_) {
					step.position.y += skillSize_.y * 0.5f + 5.0f;
				}
			}
			if (!sameHeightInfo_.empty()) {
				for (auto& sameHeight : sameHeightInfo_) {
					sameHeight.position.y += skillSize_.y * 0.5f + 5.0f;
				}
			}
			SpriteInfo info{};
			info.position = {};
			info.color = { 1.0f,1.0f,1.0f,1.0f };
			info.time = 0.0f;
			info.isAlive = true;
			info.isAnimation = true;
			sameHeightInfo_.emplace_back(info);
			isSkillSprite_ = true;
		}

	}

	for (auto& step : stepInfo_) {
		if (step.isAnimation) {
			float t = step.time / kSkillAnimationTime_;
			step.position.x = Math::Lerp(t, startSkillAnimationPos_.x, endSkillAnimationPos_.x);
			step.position.y = Math::Lerp(t, startSkillAnimationPos_.y, endSkillAnimationPos_.y);
			step.time += 1.0f;
			if (step.time >= kSkillAnimationTime_) {
				step.isAnimation = false;
				step.time = 0.0f;
				step.position = endSkillAnimationPos_;
			}
		}
		else {
			float t = step.time / kSkillColorTime_;
			step.color.w = Math::Lerp(t, 1.0f, 0.0f);
			step.time += 1.0f;
			if (step.time >= kSkillColorTime_) {
				step.isAlive = false;
				step.time = 0.0f;
			}
		}
	}
	stepInfo_.erase(std::remove_if(stepInfo_.begin(), stepInfo_.end(),
		[](const SpriteInfo& sprite) { return !sprite.isAlive; }), stepInfo_.end());
	for (auto& sameHeight : sameHeightInfo_) {
		if (sameHeight.isAnimation) {
			float t = sameHeight.time / kSkillAnimationTime_;
			sameHeight.position.x = Math::Lerp(t, startSkillAnimationPos_.x, endSkillAnimationPos_.x);
			sameHeight.position.y = Math::Lerp(t, startSkillAnimationPos_.y, endSkillAnimationPos_.y);
			sameHeight.time += 1.0f;
			if (sameHeight.time >= kSkillAnimationTime_) {
				sameHeight.isAnimation = false;
				sameHeight.time = 0.0f;
			}
		}
		else {
			float t = sameHeight.time / kSkillColorTime_;
			sameHeight.color.w = Math::Lerp(t, 1.0f, 0.0f);
			sameHeight.time += 1.0f;
			if (sameHeight.time >= kSkillColorTime_) {
				sameHeight.isAlive = false;
				sameHeight.time = 0.0f;
			}
		}
	}
	sameHeightInfo_.erase(std::remove_if(sameHeightInfo_.begin(), sameHeightInfo_.end(),
		[](const SpriteInfo& sprite) { return !sprite.isAlive; }), sameHeightInfo_.end());
}

void Player::SkillDraw() {
	for (auto& sameHeight : sameHeightInfo_) {
		TOMATOsEngine::DrawSpriteRectAngle(sameHeight.position, skillSize_, { 0.5f,0.5f }, 0.0f, {}, { 128.0f,64.0f }, sameHeightTextureHandle_, Color::Convert(sameHeight.color));
	}
	for (auto& step : stepInfo_) {
		TOMATOsEngine::DrawSpriteRectAngle(step.position, skillSize_, { 0.5f,0.5f }, 0.0f, {}, { 128.0f,64.0f }, stepTextureHandle_, Color::Convert(step.color));
	}
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
				isComboed_ = false;
				isSkillSprite_ = false;
				stepCount_++;
				comboDrawCount_ = 0;
				isHorizontal_ = false;
				if (stepCount_ >= 1) {
					size_t playHandle = TOMATOsEngine::PlayAudio(comboSoundHandle_);
					TOMATOsEngine::SetPitch(playHandle, 1.0f + stepCount_ * 0.1f);
				}
			}
			else {
				// 一コンボ目
				isComboed_ = false;
				isSkillSprite_ = false;
				stepCount_ = 0;
				comboDrawCount_ = 0;
			}
			preStep_ = step_;
#pragma endregion
#pragma region 平行
			sameHeightY_ = blockIndexY;
			sameHeightX_ = blockIndexX;
			if (sameHeightY_ == preSameHeightY_ &&
				sameHeightX_ != preSameHeightX_) {
				isDifferentX_ = true;
				isComboed_ = true;
				sameHeightCount_++;
				stepCount_ = 0;
				isHorizontal_ = true;
				// 2コンボめ以上から音が鳴る
				if (sameHeightCount_ >= 1) {
					size_t playHandle = TOMATOsEngine::PlayAudio(comboSoundHandle_);
					TOMATOsEngine::SetPitch(playHandle, 1.0f + sameHeightCount_ * 0.1f);
				}
			}
			else {
				// 一コンボ目
				isDifferentX_ = true;
				isComboed_ = false;
				isSkillSprite_ = false;
				sameHeightCount_ = 0;
				comboDrawCount_ = 0;
			}
			if (isDifferentX_ &&
				sameHeightX_ == preSameHeightX_) {
				isDifferentX_ = false;
			}
			preSameHeightY_ = sameHeightY_;
			preSameHeightX_ = sameHeightX_;
		}
		else {
			// 地面
			isDifferentX_ = false;
			size_t playHandle = TOMATOsEngine::PlayAudio(groundJumpSoundHandle_);
			TOMATOsEngine::SetVolume(playHandle, 0.8f);
			stepCount_ = -1;
			sameHeightCount_ = -1;
		}
	}
	else {
		break_ = false;
	}

	// 縦のライン
	float stepCombo = std::clamp(static_cast<float>(sameHeightCount_), -1.0f, 2.0f);
	float sameHeightCombo = std::clamp(static_cast<float>(stepCount_), -1.0f, 2.0f);
	float combo = std::max(stepCombo, sameHeightCombo);
	if (combo >= 0) {
		const uint32_t kNum = 8;
		const Vector2 size = { float(TOMATOsEngine::kMonitorWidth / kNum),float(TOMATOsEngine::kMonitorWidth / kNum) * 2 };
		const float distance = size.y + size.y * 2.0f;
		float verticalLineColorH_ = 0.0f;
		float verticalLineColorS_ = 0.0f;
		float verticalLineColorV_ = 0.0f;

		if (stepCount_ > sameHeightCount_) {
			if (stepCount_ == 0) {
				verticalLineColorS_ = kCombo1S_;
				verticalLineColorV_ = kCombo1V_;
				verticalLineColorH_ = stepColorH_;
			}
			else if (stepCount_ == 1) {
				verticalLineColorS_ = kCombo2S_;
				verticalLineColorV_ = kCombo2V_;
				verticalLineColorH_ = stepColorH_;
			}
			else {
				verticalLineColorS_ = kCombo3S_;
				verticalLineColorV_ = kCombo3V_;
				verticalLineColorH_ = h_;
			}
		}
		else {
			if (sameHeightCount_ == 0) {
				verticalLineColorS_ = kCombo1S_;
				verticalLineColorV_ = kCombo1V_;
				verticalLineColorH_ = sameHeightColorH_;
			}
			else if (sameHeightCount_ == 1) {
				verticalLineColorS_ = kCombo2S_;
				verticalLineColorV_ = kCombo2V_;
				verticalLineColorH_ = sameHeightColorH_;
			}
			else {
				verticalLineColorS_ = kCombo3S_;
				verticalLineColorV_ = kCombo3V_;
				verticalLineColorH_ = h_;
			}
		}
		// 足さないと一コンボでも0でfor文の中に入らない
		combo += 1.0f;
		for (uint32_t ix = 0; ix < kNum; ix++) {
			for (uint32_t iy = 0; iy < uint32_t(combo); iy++) {
				particleManager_->GetVerticalLine()->Create(
					Vector2(
						((float(ix) * size.x + (size.x * 0.5f))),
						static_cast<float>(iy) * -distance
					),
					Color::HSVA(verticalLineColorH_, verticalLineColorS_, verticalLineColorV_, 0.1f),
					static_cast<uint32_t>(VerticalLine::Texture::kHorizonLine),
					{ size.x * 0.5f,size.y * 2.0f }
				);
			}
		}
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
	yosokuColor_ = 0xFFFFFFFF;
	if (blockIndexY != -1) {
		const float AddX = 40.0f;
		if (sameHeightStart_) {
			sameHeightColorChangePositionRight_.x += AddX;
			sameHeightColorChangePositionLeft_.x -= AddX;
		}
		if (sameHeightColorChangePositionRight_.x >= float(Field::kNumHorizontalBlocks * Field::kBlockSize) + float(Field::kBlockSize) &&
			sameHeightColorChangePositionLeft_.x <= -float(Field::kBlockSize)) {
			sameHeightStart_ = false;
		}
		for (uint32_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
			// ライン
			if (sameHeightStart_) {
				int32_t posX = int32_t(x) * int32_t(Field::kBlockSize) + int32_t(Field::kBlockSize / 2);
				if (int32_t(sameHeightColorChangePositionRight_.x) > posX &&
					int32_t(sameHeightColorChangePositionLeft_.x) < posX) {

					// 平行(パーティクル)
					if (!sameHeightParticleFlag_.at(x)) {
						if (sameHeightCount_ == 0) {
							//// 色
							//const float kS = 0.5f;
							//const float kV = 0.3f;
							particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float(blockIndexY * Field::kBlockSize) + (Field::kBlockSize / 2)), { 1.0f,1.0f,1.0,0.1f }, static_cast<uint32_t>(Circle::Texture::kSquare));
							sameHeightParticleFlag_.at(x) = true;
						}
						else if (sameHeightCount_ == 1) {
							//// 色
							//const float kS = 0.5f;
							//const float kV = 0.3f;
							particleManager_->GetCircle()->Create(Vector2(float(x * Field::kBlockSize) + (Field::kBlockSize / 2), float(blockIndexY * Field::kBlockSize) + (Field::kBlockSize / 2)), { 1.0f,1.0f,1.0,0.1f }, static_cast<uint32_t>(Circle::Texture::kSquare));
							sameHeightParticleFlag_.at(x) = true;
						}
					}
					//// 平行(ブロックの色)
					//if (field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY)) == Field::Normal) {
					//	if (sameHeightCount_ == 0) {
					//		// 色
					//		field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo1S_, kCombo1V_));
					//	}
					//	else {
					//		// 色
					//		field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo2S_, kCombo2V_));
					//	}
					//}

				}
			}
			// フィーバーかどうか
			else if (FeverManager::GetInstance()->GetIsFever()) {
				// フィーバー中は虹色
				for (uint32_t y = 0; y < Field::kNumVerticalBlocks; y++) {
					if (field_->GetBlock(x, y) == Field::Normal) {
						// 色
						const float kAddH = 0.0008f;
						h_ += kAddH;
						if (h_ >= 1.0f) {
							h_ = 0.0f;
						}

						field_->SetColorBlock(x, y, Color::HSVA(h_, kCombo3S_, kCombo3V_));
						yosokuColor_ = Color::HSVA(h_, kCombo3S_, kCombo3V_);
					}
				}
			}
			// コンボ達成しているか
			else if (stepCount_ < kCombo_ && sameHeightCount_ < kCombo_) {
				// 階段
				if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 2)) == Field::None && field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY + 1)) == Field::Normal) {
					if (stepCount_ == 0) {
						// 色
						field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY + 1), Color::HSVA(stepColorH_, kCombo1S_, kCombo1V_));
						yosokuColor_ = Color::HSVA(stepColorH_, kCombo1S_, kCombo1V_);
					}
					else if (stepCount_ == 1) {
						// 色
						field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY + 1), Color::HSVA(stepColorH_, kCombo2S_, kCombo2V_));
						yosokuColor_ = Color::HSVA(stepColorH_, kCombo2S_, kCombo2V_);
					}
				}
				// 平行
				if (field_->GetBlock(x, static_cast<uint32_t>(blockIndexY + 1)) == Field::None &&
					field_->GetBlock(static_cast<uint32_t>(x), static_cast<uint32_t>(blockIndexY)) == Field::Normal &&
					isDifferentX_) {
					if (sameHeightCount_ == 0) {
						// 色
						field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo1S_, kCombo1V_));
						yosokuColor_ = Color::HSVA(sameHeightColorH_, kCombo1S_, kCombo1V_);
					}
					else if (sameHeightCount_ == 1) {
						// 色
						field_->SetColorBlock(x, static_cast<uint32_t>(blockIndexY), Color::HSVA(sameHeightColorH_, kCombo2S_, kCombo2V_));
						yosokuColor_ = Color::HSVA(sameHeightColorH_, kCombo2S_, kCombo2V_);
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
						yosokuColor_ = Color::HSVA(h_, kCombo3S_, kCombo3V_);
					}
				}
			}
		}
	}
	else {
		// フィーバーかどうか
		if (FeverManager::GetInstance()->GetIsFever()) {
			// フィーバー中は虹色
			for (uint32_t x = 0; x < Field::kNumHorizontalBlocks; x++) {
				for (uint32_t y = 0; y < Field::kNumVerticalBlocks; y++) {
					if (field_->GetBlock(x, y) == Field::Normal) {
						// 色
						const float kAddH = 0.0008f;
						h_ += kAddH;
						if (h_ >= 1.0f) {
							h_ = 0.0f;
						}
						field_->SetColorBlock(x, y, Color::HSVA(h_, kCombo3S_, kCombo3V_));
						yosokuColor_ = Color::HSVA(h_, kCombo3S_, kCombo3V_);
					}
				}
			}
		}
	}
}

void Player::SetBlockParticleColor(int32_t blockIndexY) {
	if (blockIndexY != -1) {
		if (isComboed_) {
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
	}
}

