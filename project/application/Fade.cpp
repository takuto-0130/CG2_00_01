#include "Fade.h"
#include "TextureManager.h"
#include "SpriteBasis.h"
#include <algorithm>
#include "imgui.h"

void Fade::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("Resources/black1x1.png");
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Resources/black1x1.png");
	sprite_->SetSize(Vector2{ 1280.0f,720.0f });
	sprite_->SetTextureSize(Vector2{ 1280,720 });
}

void Fade::Update()
{
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:
		FadeInUpdate();
		break;
	case Status::FadeOut:
		FadeOutUpdate();
		break;
	}
}

void Fade::Draw()
{
	if (status_ != Status::None) {
		SpriteBasis::GetInstance()->BasisDrawSetting();
		sprite_->Draw();
	}
}

void Fade::Start(Status status, float duration)
{
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop()
{
	status_ = Status::None;
}

bool Fade::IsFinished() const
{
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

void Fade::FadeOutUpdate()
{
	counter_ += 1.0f / 60.0f;
	if (counter_ >= duration_) {
		counter_ = duration_;
	}
	sprite_->SetColor(Vector4{ 1,1,1,std::clamp(counter_ / duration_, 0.0f, 1.0f) });
	sprite_->Update();
}

void Fade::FadeInUpdate()
{
	counter_ += 1.0f / 60.0f;
	if (counter_ >= duration_) {
		counter_ = duration_;
		Stop();
	}
	sprite_->SetColor(Vector4{ 1,1,1,1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f) });
	sprite_->Update();
}
