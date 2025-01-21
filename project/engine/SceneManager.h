#pragma once
#include "IScene.h"
class SceneManager
{
public:
	static SceneManager* GetInstance()
	{
		static SceneManager instance; // 静的ローカル変数（寿命がプログラム全体に渡る）
		return &instance;
	}

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(SceneManager&) = default;
	SceneManager& operator=(SceneManager&) = default;
public:

	// 次シーン予約
	void SetNextScene(IScene* nextScene) { nextScene_ = nextScene; }

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	IScene* scene_ = nullptr;
	IScene* nextScene_ = nullptr;
};

