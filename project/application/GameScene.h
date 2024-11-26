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
	/// <summary>
	/// re-ruのスポーン()
	/// </summary>
	void PopRail(Vector3 position, Vector3 rota) {

		Rail* rail = new Rail();
		rail->Initialize(model_.get(), position);
		rail->SetRotate(rota);
		rail->UpdateTransform();

		rails_.push_back(rail);
	}

	void PopEnemy();

	void RailCustom();

	void RailLineReDraw();

	void RailReDraw();

	void RailCameraMove();

	void RailCameraDebug();

	void SetSegment();

	void ResetRailCamera();

	void Collision();


private: // メンバ変数
	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Sprite> reticle_;

	std::array<std::unique_ptr<Sprite>, 2> lasers_;

	std::list<Rail*> rails_;

	std::unique_ptr<Model> enemyModel_;
	std::list<Enemy*> enemys_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Model> skydomeModel_;

	Camera* railCamera_ = nullptr;

	//ColliderManager* colliderManager_ = nullptr;

	std::unique_ptr<score> scoreDraw_;
	std::unique_ptr<Sprite> one_;

	std::vector<Vector3> controlPoints_;
	std::vector<Vector3> pointsDrawing_;
	size_t oneSegmentCount = 20;
	size_t segmentCount = oneSegmentCount;

	const float kDivisionSpan = 200.0f;
	float cameraSegmentCount = 1.0f / 600.0f;
	float cameraEyeT = 0;
	float cameraForwardT = 30.0f / 600.0f;

	bool isRailCameraMove_ = false;

	int32_t score_ = 0;
	const int32_t increaseScore_ = 300;
};

