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
#include "../engine/Audio/Audio.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG_

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init() {
	input_ = Input::GetInstance();

	Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);
	/*Audio::GetInstance()->LoadWave("BGM_2");
	int num = Audio::GetInstance()->PlayWave("BGM_2");
	Audio::GetInstance()->SetBGMVolume(num, 1.0f);*/

	worldTransform_.Initialize();
	ModelManager::GetInstance()->LoadModel("Resources", "cube.obj");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
}

#pragma region // 初期化以外
void GameScene::Update() {


#ifdef _DEBUG

	ImGui::Begin("Sound Test");
	ImGui::DragFloat("pitch", &pitch_, 0.01f);
	ImGui::Checkbox("isEffect", &isEffect_);
	ImGui::Text("Space ReStartStreaming");
	ImGui::End();
#endif // _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StartStreaming("BGM_2.wav", true);
	}
	Audio::GetInstance()->SetPitch(pitch_);
	if(isEffect_)
	{
		Audio::GetInstance()->SetEffect(XAUDIO2FX_I3DL2_PRESET_UNDERWATER);
	}
	else {
		Audio::GetInstance()->DisableEffect();
	}
	ParticleClass::GetInstance()->Update();
}

void GameScene::Draw() {
#pragma region 背景
	// 背景スプライト描画前
	//SpriteBasis::GetInstance()->BasisDrawSetting();

	// ↓背景

#pragma endregion

#pragma region 3Dオブジェクト
	// 3Dオブジェクト描画前
	//Object3dBasis::GetInstance()->BasisDrawSetting();
	//obj_->Draw(worldTransform_);
#pragma endregion

	ParticleClass::GetInstance()->Draw();

#pragma region 前景
	// 前景スプライト描画前
	//SpriteBasis::GetInstance()->BasisDrawSetting();
	/// ↓前景
#pragma endregion
}

void GameScene::CheckAllCollisions()
{
}

