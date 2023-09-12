#include "GameTime.h"
#include "TOMATOsEngine.h"
#include <algorithm>

GameTime::GameTime() {
    textureHandle_ = TOMATOsEngine::LoadTexture("Resources/dekisokonai36.png");
    size_ = { 36.0f,36.0f };
    pos_ = { TOMATOsEngine::kMonitorWidth / 2.0f,TOMATOsEngine::kMonitorHeight - size_.y / 2.0f - 40.0f };
    time_ = 0;
    frameTime_ = 60;
    isFinish_ = false;
    digit_ = 1;
    int tmp = time_;
    while (!(tmp < 10)) {
        tmp = tmp / 10;
        digit_++;
    }
    bgmSoundHandle_ = TOMATOsEngine::LoadAudio("Resources/Audio/ingameBGM.wav");
}

GameTime* GameTime::GetInstance() {
    static GameTime instance;
    return &instance;
}

void GameTime::Initialize()
{
    ResetTime();
    isFinish_ = false;
    bgmPlayHandle_ = TOMATOsEngine::PlayAudio(bgmSoundHandle_, true);
    TOMATOsEngine::SetVolume(bgmPlayHandle_, 1.1f);
}

void GameTime::Update()
{
    frameTime_--;
    if (frameTime_ <= 0) {
        frameTime_ = 60;
        time_++;
    }
    digit_ = 1;
    int tmp = time_;
    while (!(tmp < 10)) {
        tmp = tmp / 10;
        digit_++;
    }
}

void GameTime::Draw()
{
   /* Vector2 min;
    Vector2 max;

    if (digit_ % 2 == 0) {
        int tmp = time_;
        for (size_t i = 0; i < digit_; i++) {
            Vector2 pos = { pos_.x - size_.x / 2.0f + size_.x * i - (digit_ / 2.0f - 1) * size_.x, pos_.y };
            min = { pos.x - size_.x / 2.0f,pos.y - size_.y / 2.0f };
            max = { pos.x + size_.x / 2.0f,pos.y + size_.y / 2.0f };
            int num = static_cast<int>(tmp / (1 * std::pow(10, digit_ - i - 1)));
            tmp = tmp % static_cast<int>(1 * std::pow(10, digit_ - i - 1));
            TOMATOsEngine::DrawSpriteRect(min, max, { 36.0f * num  ,0.0f }, { 36.0f,36.0f }, textureHandle_, 0xFFFFFFFF);
        }
    }
    else {
        int tmp = time_;
        for (size_t i = 0; i < digit_; i++) {
            Vector2 pos = { pos_.x + size_.x * i - static_cast<int>(digit_ / 2.0f) * size_.x, pos_.y };
            min = { pos.x - size_.x / 2.0f,pos.y - size_.y / 2.0f };
            max = { pos.x + size_.x / 2.0f,pos.y + size_.y / 2.0f };
            int num = static_cast<int>(tmp / (1 * std::pow(10, digit_ - i - 1)));
            tmp = tmp % static_cast<int>(1 * std::pow(10, digit_ - i - 1));
            TOMATOsEngine::DrawSpriteRect(min, max, { 36.0f * num ,0.0f }, { 36.0f,36.0f }, textureHandle_, 0xFFFFFFFF);
        }
    }*/
}

void GameTime::RaiseTheBGMPitch() {
    const float upperPitch = 1.4f;
    TOMATOsEngine::SetPitch(bgmPlayHandle_, upperPitch);
}

void GameTime::LowerTheBGMPitch() {
    const float lowerPitch = 1.0f;
    TOMATOsEngine::SetPitch(bgmPlayHandle_, lowerPitch);
}

void GameTime::StopBGM() {
    TOMATOsEngine::StopAudio(bgmPlayHandle_);
}

bool GameTime::IsHurryTime() const {
    return time_ <= kHurryStartTime;
}
