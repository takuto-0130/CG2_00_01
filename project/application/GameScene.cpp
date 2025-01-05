#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "mathFunc.h"
#include "operatorOverload.h"
#include "ModelManager.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "ParticleClass.h"
#include <fstream>
#include <istream>
#include "Audio.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG_

GameScene::GameScene() {}

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Initialize(Camera* camera) {
	Audio::GetInstance()->Initialize();
	input_ = Input::GetInstance();

	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();


	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->Update();

	//=============== エネミー ===============//
	enemyGroup_ = std::make_unique<EnemyManager>();
	enemyGroup_->Initialize();
	enemyGroup_->SetPlayer(player_.get());

	// =============== 地面 =============//
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	
}

#pragma region // 初期化以外
void GameScene::Update() {

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemyGroup_->Update();

#ifdef _DEBUG
#endif // _DEBUG
}

void GameScene::Draw() {
#pragma region 背景
	// 背景スプライト描画前
	SpriteBasis::GetInstance()->BasisDrawSetting();

	// ↓背景

#pragma endregion

#pragma region 3Dオブジェクト
	// 3Dオブジェクト描画前
	Object3dBasis::GetInstance()->BasisDrawSetting();


#pragma endregion

#pragma region 前景
	// 前景スプライト描画前
	SpriteBasis::GetInstance()->BasisDrawSetting();
	/// ↓前景
#pragma endregion
}

void GameScene::CheckAllCollisions()
{
	// 衝突マネージャーのリセット
	collisionManager_->Reset();

	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());

	// コライダーリストに登録
	collisionManager_->AddCollider(player_->GetWeapon());

	// 敵全てについて
	 // 敵全てのコライダーをリストに登録
	auto enemyColliders = enemyGroup_->GetColliders();
	for (auto& collider : enemyColliders) {
		collisionManager_->AddCollider(collider);
	}


	// 衝突判定と応答
	collisionManager_->CheckAllCollisions();
}

