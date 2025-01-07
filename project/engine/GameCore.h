#pragma once
#include "TYFrameWork.h"
#include "IScene.h"

class GameCore : public TYFrameWork
{
public: // メンバ変数

	// 初期化
	void Initialize() override;

	void AddGlobalVariables();

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

private:

	GlobalVariables* globalVar_ = nullptr;

	std::unique_ptr<IScene> sceneArr_[3];
	Input* input = nullptr;

	int currentSceneNo_;
	int prevSceneNo_;
};

