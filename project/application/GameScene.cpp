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

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init() {
	input_ = Input::GetInstance();

	/*Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);*/
	Audio::GetInstance()->LoadWave("BGM_2");
	int num = Audio::GetInstance()->PlayWave("BGM_2");
	Audio::GetInstance()->SetBGMVolume(num, 1.0f);
	Audio::GetInstance()->LoadWave("BGM_2");
	int num2 = Audio::GetInstance()->PlayWave("BGM_2");
	Audio::GetInstance()->SetBGMVolume(num2, -0.9f);
}

#pragma region // 初期化以外
void GameScene::Update() {

	// 衝突判定と応答
	CheckAllCollisions();

	// プレイヤーの更新
	player_->Update();

	camera_->FollowCamera(player_->GetPosition());
	ground_->Update();

	// 敵の更新
	enemyGroup_->Update();


	if (input_->TriggerKey(DIK_RETURN)) {
		sceneNo_ = CLEAR;
	}

#ifdef _DEBUG

	ImGui::Begin("a");
	ImGui::DragFloat("pitch", &pitch_, 0.01f);
	ImGui::End();
#endif // _DEBUG
	Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->SetEffect(XAUDIO2FX_I3DL2_PRESET_UNDERWATER);
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
	player_->Draw();
	enemyGroup_->Draw();
	ground_->Draw();

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

