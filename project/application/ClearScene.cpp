#include "ClearScene.h"
#include "imgui.h"

void ClearScene::Init() { input_ = Input::GetInstance(); }

void ClearScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo_ = TITLE;
	}
#ifdef _DEBUG
	ImGui::Begin("CLEAR");
	ImGui::End();
#endif // _DEBUG
}

void ClearScene::Draw() {
}
