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

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG_

GameScene::~GameScene() {
	//Audio::GetInstance()->StopStreaming();
}

void GameScene::Init() {
	input_ = Input::GetInstance();

	audio = AudioManager::GetInstance();

	//audio->LoadWave("BGM_2");
	audio->LoadWave("fanfare");

	//bgmId = audio->PlayWave("BGM_2", AudioType::kBGM);
	seId = audio->PlayWave("fanfare", AudioType::kGameSE, true);

	audio->StartStreaming("BGM_2.wav", true);
	/*Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);*/
	/*Audio::GetInstance()->LoadWave("BGM_2");
	num = Audio::GetInstance()->PlayWave("BGM_2");
	Audio::GetInstance()->SetAudioVolume(num, 1.0f);

	Audio::GetInstance()->LoadWave("fanfare");
	num2 = Audio::GetInstance()->PlayWave("fanfare", true);
	Audio::GetInstance()->SetAudioVolume(num2, 1.0f);*/
}

#pragma region // 初期化以外
void GameScene::Update() {


#ifdef _DEBUG

	ImGui::Begin("Sound Test");
	//ImGui::DragFloat("pitch", &pitch_, 0.01f);
	ImGui::DragFloat("masterVolume", &masterVolume, 0.01f);
	ImGui::DragFloat("volume", &volume, 0.01f);
	ImGui::DragFloat("volume2", &volume2, 0.01f);
	ImGui::DragFloat("BGMVolume", &BGMVolume, 0.01f);
	ImGui::DragFloat("SEVolume", &SEVolume, 0.01f);
	//ImGui::Checkbox("isEffect", &isEffect_);
	//ImGui::Text("Space ReStartStreaming");
	ImGui::End();
	audio->SetMasterVolume(masterVolume);
	audio->SetSubmixVolume(kBGM, BGMVolume);
	audio->SetSubmixVolume(kGameSE, SEVolume);
	//audio->SetSoundVolume(bgmId, volume);
	audio->SetSoundVolume(seId, volume2);
	/*Audio::GetInstance()->SetAudioVolume(num, volume);
	Audio::GetInstance()->SetAudioVolume(num2, volume2);
	Audio::GetInstance()->SetSubmixVolume(submixVolume);*/
#endif // _DEBUG
	/*if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StartStreaming("BGM_2.wav", true);
	}
	Audio::GetInstance()->SetPitch(pitch_);
	if(isEffect_)
	{
		Audio::GetInstance()->SetEffect(XAUDIO2FX_I3DL2_PRESET_UNDERWATER);
	}
	else {
		Audio::GetInstance()->DisableEffect();
	}*/
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
}

