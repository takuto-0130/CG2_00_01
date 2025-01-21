#include "TitleScene.h"
#include "imgui.h"

void TitleScene::Init() { input_ = Input::GetInstance(); }

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo_ = STAGE;
	}
#ifdef _DEBUG
	ImGui::Begin("TITLE");
	ImGui::Text("Space to GameScene");
	ImGui::End();
#endif // _DEBUG

}

void TitleScene::Draw() {
}
