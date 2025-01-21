#include "SceneManager.h"

SceneManager::~SceneManager()
{
	delete scene_;
}

void SceneManager::Update()
{
	// シーン切り替え機構
	// 次シーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			delete scene_;
		}

		// シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		// シーンの初期化
		scene_->Init();
	}

	// 実行中シーンを更新する
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();
}
