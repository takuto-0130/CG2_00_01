#include "ClearScene.h"
#include "imgui.h"

void ClearScene::Init() { 
	input_ = Input::GetInstance();
	phase_ = ClearPhase::kFadeIn;
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, 2.0f);
}

void ClearScene::Update() {
	fade_->Update();

#ifdef _DEBUG
	ImGui::Begin("CLEAR");
	ImGui::End();
#endif // _DEBUG

	ChangePhase();
}

void ClearScene::Draw() {

	fade_->Draw();
}

void ClearScene::ChangePhase()
{
	switch (phase_) {
	case ClearPhase::kClear:

		if (input_->TriggerKey(DIK_SPACE)) {
			phase_ = ClearPhase::kFadeOut;
			fade_->Start(Status::FadeOut, 2.0f);
		}

		break;
	case ClearPhase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = ClearPhase::kClear;
		}
		break;
	case ClearPhase::kFadeOut:
		if (fade_->IsFinished()) {
			// シーンの切り替え依頼
			sceneNo_ = TITLE;
		}
		break;

	}
}
