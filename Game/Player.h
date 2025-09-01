#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Field;
class ParticleManager;

class Player {
public:

	void Initialize();
	void Update();
	void move();
	void Draw();
	void ComboDraw();

	void SetPosition(const Vector2& position) { position_ = position; }
	void SetField(Field* field) { field_ = field; }
	void SetParticleManager(ParticleManager* particleManager) { particleManager_ = particleManager; }

	int32_t GetStepCount() const { return stepCount_; }
	int32_t GetSameHeightCount() const { return sameHeightCount_; }
	float GetStepColorH() const { return stepColorH_; }
	float GetSameHeightColorH() const { return sameHeightColorH_; }
	float GetRainbowColorH() const { return h_; }
	bool GetIsEndGameClearEasing()const { return isEndGameClearEasing_; }
	Vector2 GetVelocity()const { return velocity_; }

private:
	static const int32_t kAnimationSwitchNum = 10;

	void ComboUpdate(float floor, uint32_t x, uint32_t y);
	void SetBlockColor(int32_t y);
	void SetBlockParticleColor(int32_t y);
	void CreateParticle(uint32_t x, uint32_t y);
	void SetColorChange(const Vector2& position, uint32_t nowHeight);
	void GameOverUpdate();
	void GameClearUpdate();
	void SkillUpdate();
	void SkillDraw();

	// 真ん中
	Vector2 position_;
	Vector2 size_;

	Vector2 velocity_;
	// コンボ
	const int32_t kCombo_ = 2;
	std::array<TextureHandle, 3> comboTextureHandle_;
	uint32_t comboDrawCount_;
	float comboDrawAngle_;
	Vector2 comboDrawSize_;

	Vector2 comboPosition_;
	Vector2 comboSize_;
	float comboColorH_;
	bool break_;
	// コンボ1
	const float kCombo1S_ = 1.0f;
	const float kCombo1V_ = 0.3f;
	// コンボ2
	const float kCombo2S_ = 1.0f;
	const float kCombo2V_ = 1.0f;
	// コンボ3
	const float kCombo3S_ = 1.0f;
	const float kCombo3V_ = 1.0f;
	// 虹色
	float h_;
	Vector4 bonusColor_;
	// 階段
	uint32_t preStep_;
	uint32_t step_;
	int32_t stepCount_;
	float stepColorH_;
	// 平行
	uint32_t preSameHeightX_;
	uint32_t sameHeightX_;
	bool isDifferentX_;
	uint32_t preSameHeightY_;
	uint32_t sameHeightY_;
	int32_t sameHeightCount_;
	float sameHeightColorH_;
	Vector2 sameHeightColorChangePositionRight_;
	Vector2 sameHeightColorChangePositionLeft_;
	Vector2 sameHeightColorChangeVelocity_;
	std::array<bool, 16>  sameHeightParticleFlag_;
	bool sameHeightStart_;

	bool isComboed_ = false;

	int32_t preHeight_;
	int32_t nowHeight_;

	int32_t nowWeight_;

	// テクスチャハンドル
	TextureHandle textureHandle_;
	// テクスチャハンドル
	TextureHandle clearTextureHandle_;
	// 技テクスチャハンドル
	struct SpriteInfo {
		Vector2 position;
		Vector4 color;
		float time;
		bool isAnimation;
		bool isAlive;
	};
	TextureHandle sameHeightTextureHandle_;
	TextureHandle stepTextureHandle_;
	float kSkillAnimationTime_ = 20.0f;
	float kSkillColorTime_ = 240.0f;
	Vector2 startSkillAnimationPos_ = {-128.0f,180.0f};
	Vector2 endSkillAnimationPos_ = { 128.0f*0.5f+10.0f,180.0f };
	Vector2 skillSize_={ 128.0f,64.0f };
	std::vector<SpriteInfo> sameHeightInfo_;
	std::vector<SpriteInfo> stepInfo_;
	bool isSkillSprite_=false;
	//animation
	int32_t animationFrame_;
	uint32_t continueTextureNum_;

	size_t comboSoundHandle_;
	size_t groundJumpSoundHandle_;
	//gameover
	Vector2 gameOverVelocity_;
	float gameOverAngle_;
	//gameClear
	bool isSaveClearPos_;
	float gameClearT_;
	Vector2 gameClearPos_;
	Vector2 gameClearSize_;
	float gameClearRadian_;
	bool isHorizontal_;
	bool isEndGameClearEasing_;

	const int32_t kGameClearMoveCoolTime_ = 40;
	int32_t gameClearMoveCoolTime_;

	Field* field_;
	ParticleManager* particleManager_;

	Vector2 dropIndex_ = { 0.0f,0.0f };
	bool issYosoku_ = false;
	TextureHandle yosokuHandle_;
	uint32_t yosokuColor_;
};