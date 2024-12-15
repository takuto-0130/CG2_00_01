#pragma once

#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "Skydome.h"
#include "Camera.h"
#include "rail.h"
#include "Enemy.h"
#include "score.h"
#include <sstream>
#include "Audio.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private:


private: // メンバ変数

	Input* input_ = nullptr;

	float pitch_ = 1.0f;
};

