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
#include "CameraShake.h"
#include "HitStop.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG_

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init() {
	input_ = Input::GetInstance();

	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);
	globalVar_ = GlobalVariables::GetInstance();

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	// 自機
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->Update();

	// 敵集団
	enemyGroup_ = std::make_unique<EnemyManager>();
	enemyGroup_->Initialize();
	enemyGroup_->SetPlayer(player_.get());

	// 地面
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();

	phase_ = GamePhase::kFadeIn;
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, globalVar_->GetFloatValue("global", "fadeTime(sec)"));

	player_->Update();
	ApplyGlobalVariables();
}

void GameScene::ApplyGlobalVariables()
{
	const char* groupName = "global";
	clearEliminateCount_ = globalVar_->GetIntValue(groupName, "clearEliminateCount");
	camera_->SetOffsetRotate(globalVar_->GetVector3Value(groupName, "cameraOffsetR"));
	camera_->SetOffsetTranslate(globalVar_->GetVector3Value(groupName, "cameraOffsetT"));
}

#pragma region // 初期化以外
void GameScene::Update() {
	fade_->Update();

	HitStop::GetInstance()->Update();

#ifdef _DEBUG
	ImGui::Begin("GAME");
	ImGui::End();
	ApplyGlobalVariables();
#endif // _DEBUG
	ChangePhase();
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
	ground_->Draw();
	player_->Draw();
	enemyGroup_->Draw();

	ParticleClass::GetInstance()->Draw();

#pragma endregion

#pragma region 前景
	// 前景スプライト描画前
	SpriteBasis::GetInstance()->BasisDrawSetting();
	/// ↓前景

	fade_->Draw();
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

void GameScene::ChangePhase()
{
	switch (phase_) 
	{
	case GamePhase::kGame:

		if(!HitStop::GetInstance()->IsHitStop())
		{
			// 衝突判定と応答
			CheckAllCollisions();

			// プレイヤーの更新
			player_->Update();

			CameraUpdate();

			ground_->Update();

			// 敵の更新
			enemyGroup_->Update();

			if (enemyGroup_->GetEliminateCount() >= clearEliminateCount_) 
			{
				phase_ = GamePhase::kFadeOut;
				fade_->Start(Status::FadeOut, globalVar_->GetFloatValue("global", "fadeTime(sec)"));
			}
		}

		break;
	case GamePhase::kFadeIn:
		if (fade_->IsFinished()) 
		{
			phase_ = GamePhase::kGame;
		}
		camera_->FollowCamera(player_->GetPosition());
		ground_->Update();
		break;
	case GamePhase::kFadeOut:
		if (fade_->IsFinished()) 
		{
			// シーンの切り替え依頼
			sceneNo_ = CLEAR;
		}
		break;

	}
}

void GameScene::CameraUpdate()
{
	CameraShake::GetInstance()->Update();
	if (CameraShake::GetInstance()->IsShake()) {
		camera_->SetOffsetTranslate(globalVar_->GetVector3Value("global", "cameraOffsetT") + CameraShake::GetInstance()->GetShake());
	}
	camera_->FollowCamera(player_->GetPosition());
}

