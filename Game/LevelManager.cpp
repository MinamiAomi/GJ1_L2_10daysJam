#include "LevelManager.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"

#include "Field.h"
#include "GameTime.h"
#include "FeverManager.h"


void LevelManager::Initialize() {
	numLever_ = 0;
	interval_ = field_->GetGrowInterval();
	growingBlocks_ = field_->GetNumGrowingBlocks();

	textSize_ = { 512.0f,256.0f };
	textPosition_ = { float(TOMATOsEngine::kMonitorWidth) + textSize_.x,float(TOMATOsEngine::kMonitorHeight) * 0.5f };
	textVelocity_.x = -10.0f;
	textColorH_ = 0.0f;

	isTextStart_ = false;

	textureHandles_.at(Texture::kEffect) = TOMATOsEngine::LoadTexture("Resources/speedUpEffect.png");
	textureHandles_.at(Texture::kText) = TOMATOsEngine::LoadTexture("Resources/speedUp.png");

	for (auto& effect : effects_) {
		effect = std::make_unique<Effect>();
	}
}

void LevelManager::Update() {
	// レベル
	LevelUpdate();
	// エフェクト
	EffectUpdate();
}

void LevelManager::Draw() {
	Random::RandomNumberGenerator rnd{};

	const float kS = rnd.NextFloatRange(0.5f, 1.0f);
	const float kV = rnd.NextFloatRange(0.5f, 1.0f);
	for (auto& effect : effects_) {
		if (effect->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(effect->position_, effect->size_, { 0.5f,0.5f }, 0.0f, {}, { 32.0f,64.0f }, textureHandles_.at(Texture::kEffect), Color::HSVA((effect->colorH_ / 360.0f), kS, kV));
		}
	}
	if (isTextStart_) {
		TOMATOsEngine::DrawSpriteRectAngle(textPosition_, textSize_, { 0.5f,0.5f }, 0.0f, {}, { 64.0f,32.0f }, textureHandles_.at(Texture::kText), Color::HSVA((textColorH_ / 360.0f), kS, kV));
	}
}

void LevelManager::LevelUpdate() {
	// 何秒でレベル上がるか
	const uint32_t kCoolTime = 10 * 60;
	time_++;
	/*ImGui::Begin("level");
	ImGui::Text("%d", numLever_);
	int tmpInterval = interval_;
	ImGui::DragInt("interval", &tmpInterval);
	interval_ = tmpInterval;
	ImGui::End();*/
	if (time_ % kCoolTime == 0 && numLever_ <= kNumLeversMax) {
		// 難易度を何倍するか
		const float kLevel = 1.2f;
		// レベル倍率をかける
		interval_ = uint32_t(float(interval_) - (float(interval_) * (kLevel - 1.0f)));
		interval_ = std::clamp(interval_,(uint32_t)45, (uint32_t)10000);	// インターバルタイムをセット
		field_->SetGrowInterval(interval_);
		// レベルアップ
		numLever_++;
		// リセット
		time_ = 0;
		// エフェクト
		CreateEffect();
	}
	// フィーバー中
	else if (FeverManager::GetInstance()->GetIsFever()) {
		const uint32_t kInterval = 30;
		const uint32_t kGrowingBlocks = 6;
		// インターバルタイムをセット
		field_->SetGrowInterval(kInterval);
		// 出てくるブロック数
		field_->SetNumGrowingBlocks(kGrowingBlocks);
	}
	else {
		// インターバルタイムをセット
		field_->SetGrowInterval(interval_);
		// 出てくるブロック数
		field_->SetNumGrowingBlocks(growingBlocks_);
	}
}

void LevelManager::EffectUpdate() {
	Random::RandomNumberGenerator rnd{};
	for (auto& effect : effects_) {
		// 画面外に言ったらfalse
		if (effect->position_.y > float(TOMATOsEngine::kMonitorHeight) + effect->size_.y) {
			effect->isAlive_ = false;
		}
		else {
			// ポジション
			effect->position_ += effect->velocity_;
			// 色
			effect->colorH_ = rnd.NextFloatRange(0.0f, 20.0f);
		}

	}
	if (isTextStart_) {
		// テキスト
		textPosition_ += textVelocity_;
		if (textPosition_.x < -textSize_.x) {
			textPosition_ = { float(TOMATOsEngine::kMonitorWidth) + textSize_.x ,float(TOMATOsEngine::kMonitorHeight) * 0.5f };
			isTextStart_ = false;
		}
	}
}

void LevelManager::CreateEffect() {
	// テキストスタートフラグ
	isTextStart_ = true;
	Random::RandomNumberGenerator rnd{};
	const float kSpeedMin = 5.0f;
	const float kSpeedMax = 10.0f;
	Vector2 kSize = { 32.0f ,64.0f };
	float kSizeMin = 2.0f;
	float kSizeMax = 3.0f;

	const uint32_t countMax = 5;
	uint32_t count = 0;
	for (auto& effect : effects_) {
		if (count < countMax && !effect->isAlive_) {
			// 色
			effect->colorH_ = rnd.NextFloatRange(0.0f, 20.0f);
			// 速度
			float speed = rnd.NextFloatRange(kSpeedMin, kSpeedMax);
			effect->velocity_.y = speed;
			// サイズ
			effect->size_ = kSize * rnd.NextFloatRange(kSizeMin, kSizeMax);
			// 座標
			effect->position_.x = rnd.NextFloatRange(0.0f + effect->size_.x, float(TOMATOsEngine::kMonitorWidth) - effect->size_.x);
			effect->position_.y = -rnd.NextFloatRange(effect->size_.y, float(TOMATOsEngine::kMonitorHeight));

			effect->isAlive_ = true;

			count++;
		}
	}
}
