#include "score.h"
#include "SpriteBasis.h"

void score::Initialze()
{
	one_ = std::make_unique<Sprite>();
	one_->Initialize("Resources/number.png");
	one_->SetTextureSize({ 64,64 });
	one_->SetTextureLeftTop({ 128,0 });
	one_->SetPosition({ 1000,550 });
	one_->SetSize({ 64,64 });

	two_ = std::make_unique<Sprite>();
	two_->Initialize("Resources/number.png");
	two_->SetTextureSize({ 64,64 });
	two_->SetTextureLeftTop({ 64,0 });
	two_->SetPosition({ 45 + 1000,550 });
	two_->SetSize({ 64,64 });

	three_ = std::make_unique<Sprite>();
	three_->Initialize("Resources/number.png");
	three_->SetTextureSize({ 64,64 });
	three_->SetTextureLeftTop({ 192,0 });
	three_->SetPosition({ 90 + 1000,550 });
	three_->SetSize({ 64,64 });

	four_ = std::make_unique<Sprite>();
	four_->Initialize("Resources/number.png");
	four_->SetTextureSize({ 64,64 });
	four_->SetTextureLeftTop({ 0,0 });
	four_->SetPosition({ 135 + 1000,550 });
	four_->SetSize({ 64,64 });

	back_ = std::make_unique<Sprite>();
	back_->Initialize("Resources/white2x2.png");
	back_->SetSize({ 215,80 });
	back_->SetPosition({ 990,542 });
	back_->SetColor({ 0.f / 256.f,60.f / 256.f,27.f / 256.f,1.0f });
}

void score::Update()
{
	one_->Update();
	two_->Update();
	three_->Update();
	four_->Update();
	back_->Update();
}

void score::Draw()
{
	back_->Draw();
	one_->Draw();
	two_->Draw();
	three_->Draw();
	four_->Draw();
}
