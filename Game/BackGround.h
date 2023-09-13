#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Player;
class BackGround {
private:
	// 粒粒
	struct Grain {
		Vector2 position_;
		Vector2 vector_;
		Vector2 velocity_;
		Vector2 size_;
		Vector2 size_Original_;
		Vector4 color_;
		uint32_t death_Time_;
		uint32_t death_Count_;
		bool isAlive_;
	};
	// 四角
	struct Square {
		Vector2 position_;
		Vector2 size_;
		Vector2 size_Original_;
		float angle_;
		float angle_Original_;
		Vector4 color_;
		uint32_t death_Time_;
		uint32_t death_Count_;
		bool isAlive_;
	};
	// 縦のライン
	struct VerticalLine {
		Vector2 position_;
		Vector2 velocity_;
		/*Vector2 size_;
		Vector2 size_Original_;*/
		/*float angle_;
		float angle_Original_;*/
		float colorH_;
		uint32_t death_Time_;
		uint32_t death_Count_;
		bool isAlive_;
	};
public:
	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void FrameDraw();
	void Draw();

	void SetPlayer(Player* player) { player_ = player; }
private:
	// 四角
	void SquareInitialize();
	void SquareUpdate();
	void SquareDraw();
	// 縦のライン
	void VerticalLineInitialize();
	void VerticalLineUpdate();
	void VerticalLineDraw();

	void SetColorH(float& h, float& s, float& v);
private:
	// コンボ1
	const float kCombo1S_ = 1.0f;
	const float kCombo1V_ = 0.3f;
	// コンボ2
	const float kCombo2S_ = 1.0f;
	const float kCombo2V_ = 1.0f;
	// コンボ3
	const float kCombo3S_ = 1.0f;
	const float kCombo3V_ = 1.0f;

	// フレーム
	TextureHandle fream_TextureHandle_;
	Vector2 fream_Position_;
	Vector2 fream_Size_;
	float fream_ColorH_;
	// 四角
	static const uint32_t kSquare_Max = 30;
	std::array<std::unique_ptr<Square>, kSquare_Max> squares_;
	TextureHandle square_TextureHandle_;
	const uint32_t square_CoolTime_ = 300;
	uint32_t square_Count_;

	float square_ColorH_;
	float square_ColorS_;
	float square_ColorV_;
	
	// 縦のライン
	static const uint32_t kVerticalLine_Max = 30;
	std::array<std::unique_ptr<VerticalLine>, kVerticalLine_Max> verticalLines_;
	TextureHandle verticalLine_TextureHandle_;
	//uint32_t verticalLineNum_ = 8;

	Player* player_;
};

