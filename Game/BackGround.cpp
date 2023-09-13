#include "BackGround.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"
#include "Player.h"

#pragma region 色
float HueToRGB(float p, float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

void HSLToRGB(float hue, float saturation, float lightness, Vector4& rgb) {
	if (saturation == 0.0f) {
		rgb.x = rgb.y = rgb.z = lightness;
	}
	else {
		float q = (lightness < 0.5f) ? (lightness * (1.0f + saturation)) : (lightness + saturation - lightness * saturation);
		float p = 2.0f * lightness - q;

		rgb.x = HueToRGB(p, q, hue + 1.0f / 3.0f);
		rgb.y = HueToRGB(p, q, hue);
		rgb.z = HueToRGB(p, q, hue - 1.0f / 3.0f);
	}
	rgb.w = 1.0f;
}
#pragma endregion

BackGround::BackGround() {
	// フレーム
	fream_TextureHandle_ = TOMATOsEngine::LoadTexture("Resources/frame.png");
	fream_Position_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,float(TOMATOsEngine::kMonitorHeight) * 0.5f - 40.0f };
	fream_Size_ = { float(TOMATOsEngine::kMonitorWidth),float(TOMATOsEngine::kMonitorHeight) };
	fream_ColorH_ = 0.319f;
	// 四角
	for (auto& square : squares_) {
		square = std::make_unique<Square>();
		square->position_ = { 0.0f,0.0f };
		square->size_ = { 0.0f,0.0f };
		square->size_Original_ = { 0.0f,0.0f };
		square->angle_ = 0.0f;
		square->color_ = { 0.0f,0.0f,0.0f,0.0f };
		square->death_Time_ = 0;
		square->isAlive_ = false;
	}
	square_TextureHandle_ = TOMATOsEngine::LoadTexture("Resources/Particle/square.png");
	// 縦のライン
	for (auto& verticalLine : verticalLines_) {
		verticalLine = std::make_unique<VerticalLine>();
		verticalLine->position_ = { 0.0f,0.0f };
		verticalLine->velocity_ = { 0.0f,0.0f };
		/*verticalLine->size_ = { 0.0f,0.0f };
		verticalLine->size_Original_ = { 0.0f,0.0f };
		verticalLine->angle_ = 0.0f;*/
		verticalLine->colorH_ = 0.0f;
		verticalLine->death_Time_ = 0;
		verticalLine->isAlive_ = false;
	}

	verticalLine_TextureHandle_ = TOMATOsEngine::LoadTexture("Resources/Particle/horizonLine.png");
	Initialize();
}

BackGround::~BackGround() {}

void BackGround::Initialize() {
	// 四角
	SquareInitialize();
	// 縦のライン
	VerticalLineInitialize();
}

void BackGround::Update() {
	// 四角
	SquareUpdate();
	// 縦のライン
	VerticalLineUpdate();
}

void BackGround::Draw() {
	// フレーム
	TOMATOsEngine::DrawFrame(fream_Position_, fream_Size_, Vector2(0.5f, 0.5f), 0.0f, {}, fream_Size_, fream_TextureHandle_, Color::HSVA(fream_ColorH_, 1.0f, 1.0f, 0.4f));
	// 四角
	//SquareDraw();
	// 縦のライン
	//VerticalLineDraw();
}

void BackGround::FrameDraw() {
	// フレーム
	TOMATOsEngine::DrawFrame(fream_Position_, fream_Size_, Vector2(0.5f, 0.5f), 0.0f, {}, fream_Size_, fream_TextureHandle_, Color::HSVA(fream_ColorH_, 1.0f, 1.0f, 0.4f));
}

void BackGround::SquareInitialize() {
	square_Count_ = square_CoolTime_;
	square_ColorH_ = 0.0f;
}

void BackGround::SquareUpdate() {
	square_Count_--;
	if (square_Count_ <= 0) {
		for (auto& square : squares_) {
			if (!square->isAlive_) {
				Random::RandomNumberGenerator rnd{};
				const float kSize_Min = 100.0f;
				const float kSize_Max = 200.0f;
				const uint32_t kDeathTime_Min = 600;
				const uint32_t kDeathTime_Max = 1200;
				// サイズ
				float size = rnd.NextFloatRange(kSize_Min, kSize_Max);
				square->size_Original_ = { size,size };
				square->size_ = square->size_Original_;
				// ポジション
				float x = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorWidth) - size);
				float y = rnd.NextFloatRange(0.0f + size, static_cast<float> (TOMATOsEngine::kMonitorHeight) - size);
				square->position_ = { x,y };
				// 角度
				square->angle_ = rnd.NextFloatRange(0.0f * Math::ToRadian, 360.0f * Math::ToRadian);
				square->angle_Original_ = rnd.NextFloatRange(0.05f * Math::ToRadian, 0.5f * Math::ToRadian);
				// 回り方
				if (rnd.NextUIntRange(0, 1) == 1) {
					square->angle_Original_ *= -1;
				}
				// カラー
				square->color_ = { 1.0f,1.0f,1.0f,0.0f };
				// 時間
				uint32_t death_Time = rnd.NextUIntRange(kDeathTime_Min, kDeathTime_Max);
				square->death_Time_ = death_Time;
				square->death_Count_ = 0;
				// フラグ
				square->isAlive_ = true;

				break;
			}
		}
		square_Count_ = square_CoolTime_;
	}
	for (auto& square : squares_) {
		if (square->isAlive_) {
			square->death_Count_++;
			float t = std::clamp(float(square->death_Count_) / float(square->death_Time_),0.0f,1.0f);
			// 回転
			square->angle_ += square->angle_Original_;
			// 色
			SetColorH(square_ColorH_, square_ColorS_, square_ColorV_);
			// サイズ
			float size = 0.0f;
			if (t <= 0.5f) {
				size = Math::Lerp(t, 0.0f, square->size_Original_.x*2.0f);
			}
			else {
				size = Math::Lerp(t, square->size_Original_.x*2.0f, 0.0f);
			}
			square->size_ = { size,size };
			// 寿命
			if (square->death_Count_ >= square->death_Time_) {
				square->isAlive_ = false;
			}

		}

	}
}

void BackGround::SquareDraw() {

	for (auto& square : squares_) {
		if (square->isAlive_) {
			TOMATOsEngine::DrawSpriteRectAngle(square->position_, square->size_, Vector2(0.5f, 0.5f), square->angle_, {}, Vector2(32.0f, 32.0f), square_TextureHandle_, Color::HSVA(square_ColorH_, square_ColorS_, square_ColorV_,0.4f));
		}
	}
}

void BackGround::VerticalLineInitialize() {}

void BackGround::VerticalLineUpdate() {
}

void BackGround::VerticalLineDraw() {}

void BackGround::SetColorH(float& h, float& s, float& v) {
	// 地面だったら白
	if (player_->GetStepCount() == -1 || player_->GetSameHeightCount() == -1) {
		s = 0.0f;
		v = 0.5f;
	} else if (player_->GetStepCount() > player_->GetSameHeightCount()) {
		if (player_->GetStepCount() == 0) {
			s = kCombo1S_;
			v = kCombo1V_;
			h = player_->GetStepColorH();
		}
		else if (player_->GetStepCount() == 1) {
			s = kCombo2S_;
			v = kCombo2V_;
			h = player_->GetStepColorH();
		}
		else {
			s = kCombo3S_;
			v = kCombo3V_;
			h = player_->GetRainbowColorH();
		}
	}
	else  {
		if (player_->GetSameHeightCount() == 0) {
			s = kCombo1S_;
			v = kCombo1V_;
			h = player_->GetSameHeightColorH();
		}
		else if (player_->GetSameHeightCount() == 1) {
			s = kCombo2S_;
			v = kCombo2V_;
			h = player_->GetSameHeightColorH();
		}
		else {
			s = kCombo3S_;
			v = kCombo3V_;
			h = player_->GetRainbowColorH();
		}
		
	}
	
}

