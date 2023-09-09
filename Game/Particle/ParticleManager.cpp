#include "ParticleManager.h"

#include "TOMATOsEngine.h"

void ParticleManager::Initialize() { 
	// 円
	circle_ = new Circle();
	circle_->Initialize();
	// ついてく
	follow_ = new Follow();
	follow_->Initialize();
	// スプラッシュ
	splash_ = new Splash();
	splash_->Initialize();
	// ポップ
	pop_ = new Pop();
	pop_->Initialize();
	// エミッターの回りを回る
	yenLetter_ = new YenLetter();
	yenLetter_->Initialize();
}

void ParticleManager::Update() { 
	circle_->Update();
	follow_->Update();
	splash_->Update();
	pop_->Update();
	yenLetter_->Update();
}

void ParticleManager::Draw() { 
	TOMATOsEngine::SetBlendMode(kBlendModeAdditive);
	circle_->Draw();
	follow_->Draw();
	splash_->Draw();
	pop_->Draw();
	yenLetter_->Draw();
	TOMATOsEngine::SetBlendMode(kBlendModeNormal);
}
