#pragma once

#include "IScene.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "Audio.h"
#include "player/Player.h"
#include "Enemy/EnemyManager.h"
#include "global/Ground.h"

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


	std::unique_ptr<Player> player_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<EnemyManager> enemyGroup_;
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;
};

