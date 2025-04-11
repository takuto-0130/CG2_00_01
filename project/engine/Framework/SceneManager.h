#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"
class SceneManager
{
public:
	static SceneManager* GetInstance()
	{
		static SceneManager instance; // 静的ローカル変数（寿命がプログラム全体に渡る）
		return &instance;
	}

	SceneManager() = default;
	~SceneManager();
private:
	SceneManager(SceneManager&) = default;
	SceneManager& operator=(SceneManager&) = default;
public:

	// 次シーン予約
	void ChangeScene(const std::string& sceneName);

	// 更新
	void Update();

	// 描画
	void Draw();

	// シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	IScene* scene_ = nullptr;
	IScene* nextScene_ = nullptr;

	AbstractSceneFactory* sceneFactory_ = nullptr;
};

