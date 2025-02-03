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

enum class GamePhase
{
	kFadeIn,
	kGame,
	kFadeOut
};

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene 
	: public IScene 
{

public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	void ApplyGlobalVariables();

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
	void ChangePhase();

	void CameraUpdate();

private: // メンバ変数
	std::unique_ptr<Fade> fade_;
	GamePhase phase_ = GamePhase::kFadeIn;

	uint32_t clearEliminateCount_ = 50;


	std::unique_ptr<Player> player_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<EnemyManager> enemyGroup_;
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;
};

