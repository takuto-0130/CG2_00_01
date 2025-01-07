#include "TitleScene.h"
#include "SpriteBasis.h"
#include "imgui.h"

TitleScene::TitleScene()
{
	TextureManager::GetInstance()->LoadTexture("Resources/title.png");
	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize("Resources/title.png");
	titleSprite_->SetSize(Vector2{ 1280.0f,720.0f });
	titleSprite_->SetTextureSize(Vector2{ 1280,720 });
	globalVar_ = GlobalVariables::GetInstance();
}

void TitleScene::Init() {
	input_ = Input::GetInstance();
	phase_ = TitlePhase::kFadeIn; 
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, globalVar_->GetFloatValue("global", "fadeTime(sec)"));
}

void TitleScene::Update() {
	fade_->Update();
	titleSprite_->Update();
#ifdef _DEBUG
	ImGui::Begin("TITLE");
	ImGui::End();
#endif // _DEBUG

	ChangePhase();
}

void TitleScene::Draw() {

	SpriteBasis::GetInstance()->BasisDrawSetting();
	titleSprite_->Draw();
	fade_->Draw();
}

void TitleScene::ChangePhase()
{
	switch (phase_) {
	case TitlePhase::kTitle:

		if (input_->TriggerKey(DIK_SPACE)) {
			phase_ = TitlePhase::kFadeOut;
			fade_->Start(Status::FadeOut, globalVar_->GetFloatValue("global", "fadeTime(sec)"));
		}

		break;
	case TitlePhase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = TitlePhase::kTitle;
		}
		break;
	case TitlePhase::kFadeOut:
		if (fade_->IsFinished()) {
			// シーンの切り替え依頼
			sceneNo_ = STAGE;
		}
		break;
	}
}
