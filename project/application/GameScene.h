#pragma once

#include "IScene.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "../engine/Audio/Audio.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene {

public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	void CheckAllCollisions();


private:


private: // メンバ変数


	float pitch_ = 1.0f;

	Vector3 cameraOffset_;

#ifdef _DEBUG
	bool isEffect_ = false;
#endif // _DEBUG

	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;

};

