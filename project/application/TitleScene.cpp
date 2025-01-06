#include "TitleScene.h"
#include "imgui.h"

void TitleScene::Init() { 
	input_ = Input::GetInstance();
	phase_ = TitlePhase::kFadeIn; 
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, 2.0f);
}

void TitleScene::Update() {
	fade_->Update();
#ifdef _DEBUG
	ImGui::Begin("TITLE");
	ImGui::End();
#endif // _DEBUG

	ChangePhase();
}

void TitleScene::Draw() {


	fade_->Draw();
}

void TitleScene::ChangePhase()
{
	switch (phase_) {
	case TitlePhase::kTitle:

		if (input_->TriggerKey(DIK_SPACE)) {
			phase_ = TitlePhase::kFadeOut;
			fade_->Start(Status::FadeOut, 2.0f);
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
