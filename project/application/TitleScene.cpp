#include "TitleScene.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "imgui.h"

void TitleScene::Init() { input_ = Input::GetInstance(); }

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
#ifdef _DEBUG
	ImGui::Begin("TITLE");
	ImGui::Text("Space to GameScene");
	ImGui::End();
#endif // _DEBUG

}

void TitleScene::Draw() {
}
