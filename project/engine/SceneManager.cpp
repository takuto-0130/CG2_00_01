#include "SceneManager.h"

SceneManager::~SceneManager()
{
	delete scene_;
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
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

		// シーンマネージャーをセット
		scene_->SetSceneManager(this);

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
