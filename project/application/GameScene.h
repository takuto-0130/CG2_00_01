#pragma once

#include "IScene.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "../engine/Audio/AudioManager.h"

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

	float masterVolume = 1.0f;
	float BGMVolume = 1.0f;
	float SEVolume = 1.0f;
	float volume = 1.0f;
	int num = -1;
	float volume2 = 1.0f;
	int num2 = -1;

	AudioManager::AudioID bgmId;
	AudioManager::AudioID seId;

	float pitch_ = 1.0f;

	Vector3 cameraOffset_;

	AudioManager* audio = nullptr;

#ifdef _DEBUG
	bool isEffect_ = false;
#endif // _DEBUG


};

