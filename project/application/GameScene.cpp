#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "mathFunc.h"
#include "operatorOverload.h"
#include "ModelManager.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include <fstream>
#include <istream>
#include "Audio.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG_

GameScene::GameScene() {}

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
	for (Rail* rail : rails_) {
		delete rail;
		rail = nullptr;
	}
	enemys_.clear();
}

void GameScene::Initialize(Camera* camera) {
	Audio::GetInstance()->Initialize();
	input_ = Input::GetInstance();
#pragma region // スプライトやオブジェクト関連
	railCamera_ = camera;
	TextureManager::GetInstance()->LoadTexture("Resources/number.png");
	TextureManager::GetInstance()->LoadTexture("Resources/white2x2.png");
	model_ = std::make_unique<Model>();
	model_->Initialize(ModelManager::GetInstance()->GetModelLoader(), "Resources", "cube.obj");

	TextureManager::GetInstance()->LoadTexture("Resources/reticle.png");

	skydomeModel_ = std::make_unique<Model>();
	skydomeModel_->Initialize(ModelManager::GetInstance()->GetModelLoader(), "Resources", "skydome.obj");
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());
	
	enemyModel_ = std::make_unique<Model>();
	enemyModel_->Initialize(ModelManager::GetInstance()->GetModelLoader(), "Resources", "EnemyBody.obj");

	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/reticle.png");
	reticle_->SetAnchorPoint({ 0.5f,0.5f });

	for (size_t i = 0; i < 2; ++i) {
		lasers_[i] = std::make_unique<Sprite>();
		lasers_[i]->Initialize("Resources/white2x2.png");
		lasers_[i]->SetColor({ 0.1f,0.5f,0.5f,1.0f });
	}

	scoreDraw_ = std::make_unique<score>();
	scoreDraw_->Initialze();
#pragma endregion

#pragma region // レール設定
	controlPoints_ = {
		{0,  0,  0},
		{10, 10, 10},
		{10, 15, 20},
		{20, 15, 30},
		{20, 0,  35},
		{30, 0,  40},
		{30, 0,  50},
		{40, 0,  55}
	};
	segmentCount = oneSegmentCount * controlPoints_.size();
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	size_t i = 0;
	for (Vector3& v : pointsDrawing_) {
		if (pointsDrawing_[i] == pointsDrawing_.back()) {
			break;
		}
		++i;
		Vector3 rotateRail{};
		Vector3 forward = pointsDrawing_[i] - v;
		rotateRail.y = std::atan2(forward.x, forward.z);
		float length = Length({ forward.x, 0, forward.z });
		rotateRail.x = std::atan2(-forward.y, length);
		PopRail(v, rotateRail);
	}
	SetSegment();
	ResetRailCamera();
#pragma endregion
	pitch_ = 1.0f;
	Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->StartStreaming("small_wav_file.wav", true);
	/*Audio::GetInstance()->LoadWave("large_wav_file");
	int num = Audio::GetInstance()->PlayWave("large_wav_file");
	Audio::GetInstance()->SetBGMVolume(num, 1.0f);*/
}

#pragma region // 初期化以外
void GameScene::Update() {
//	RailCameraDebug();
//	RailCameraMove();
//	RailCustom();
//
//	if (input_->TriggerKey(DIK_RETURN)) {
//		enemys_.remove_if([](Enemy* enemy) {
//			if (!enemy->IsDead()) {
//				delete enemy;
//				return true;
//			}
//			return false;
//			});
//		ResetRailCamera();
//	}
//	if (input_->PushKey(DIK_SPACE)) {
//		Collision();
//	}
//	enemys_.remove_if([](Enemy* enemy) {
//		if (enemy->IsDead()) {
//			delete enemy;
//			return true;
//		}
//		return false;
//		});
//
//	PopEnemy();
//
//	skydome_->Update();
//	Vector2 mouse = input_->GetMousePosition();
//#ifdef _DEBUG
//	ImGui::Begin("a");
//	ImGui::DragFloat2("b", &mouse.x, 0.1f);
//	ImGui::InputInt("score", &score_);
//	ImGui::End();
//#endif // _DEBUG
//	for (size_t i = 0; i < 2; ++i) {
//		lasers_[i]->Update();
//	}
//	reticle_->SetPosition(mouse);
//	reticle_->Update();
//	scoreDraw_->Update();
	ImGui::Begin("a");
	ImGui::DragFloat("b", &pitch_, 0.01f);
	ImGui::End();
	Audio::GetInstance()->SetPitch(pitch_);
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

	/// ↓3Dオブジェクト

	/*skydome_->Draw();

	for (Rail* rail : rails_) {
		rail->Draw();
	}
	for (Enemy* enemy : enemys_) {
		enemy->Draw();
	}*/


#pragma endregion

#pragma region 前景
	// 前景スプライト描画前
	SpriteBasis::GetInstance()->BasisDrawSetting();

	/// ↓前景
	/*Vector2 mouse = input_->GetMousePosition();
	if(input_->PushKey(DIK_SPACE)) {
		for (size_t i = 0; i < 2; ++i) {
			lasers_[i]->DrawRect(mouse, mouse,
				{ 426.7f * float(1 + i) - 20.0f, 720 },
				{ 426.7f * float(1 + i) + 20.0f, 720 });
		}
	}
	reticle_->Draw();
	scoreDraw_->Draw();*/
#pragma endregion
}

void GameScene::PopEnemy()
{
	if (cameraEyeT > cameraSegmentCount * 200.0f &&
		cameraEyeT < cameraSegmentCount * 201.0f) {
		Enemy* enemy1 = new Enemy();
		enemy1->Initialize(enemyModel_.get(), { 10, 16, 22 });
		enemy1->UpdateTransform();
		enemys_.push_back(enemy1);

		Enemy* enemy2 = new Enemy();
		enemy2->Initialize(enemyModel_.get(), { 11, 16, 21 });
		enemy2->UpdateTransform();
		enemys_.push_back(enemy2);

		Enemy* enemy3 = new Enemy();
		enemy3->Initialize(enemyModel_.get(), { 9, 15, 19 });
		enemy3->UpdateTransform();
		enemys_.push_back(enemy3);

		Enemy* enemy4 = new Enemy();
		enemy4->Initialize(enemyModel_.get(), { 10, 17, 20 });
		enemy4->UpdateTransform();
		enemys_.push_back(enemy4);
	}

	if (cameraEyeT > cameraSegmentCount * 500.0f &&
		cameraEyeT < cameraSegmentCount * 501.0f) {
		enemys_.remove_if([](Enemy* enemy) {
			if (!enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
			});
		Enemy* enemy1 = new Enemy();
		enemy1->Initialize(enemyModel_.get(), { 21, 7, 35 });
		enemy1->UpdateTransform();
		enemys_.push_back(enemy1);

		Enemy* enemy2 = new Enemy();
		enemy2->Initialize(enemyModel_.get(), { 22, 6, 35 });
		enemy2->UpdateTransform();
		enemys_.push_back(enemy2);

		Enemy* enemy3 = new Enemy();
		enemy3->Initialize(enemyModel_.get(), { 20, 7, 35 });
		enemy3->UpdateTransform();
		enemys_.push_back(enemy3);

		Enemy* enemy4 = new Enemy();
		enemy4->Initialize(enemyModel_.get(), { 19, 8, 35 });
		enemy4->UpdateTransform();
		enemys_.push_back(enemy4);
	}

	if (cameraEyeT > cameraSegmentCount * 800.0f &&
		cameraEyeT < cameraSegmentCount * 801.0f) {
		enemys_.remove_if([](Enemy* enemy) {
			if (!enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
			});
		Enemy* enemy1 = new Enemy();
		enemy1->Initialize(enemyModel_.get(), { 25, 0,  37 });
		enemy1->UpdateTransform();
		enemys_.push_back(enemy1);

		Enemy* enemy2 = new Enemy();
		enemy2->Initialize(enemyModel_.get(), { 27, 2,  38});
		enemy2->UpdateTransform();
		enemys_.push_back(enemy2);

		Enemy* enemy3 = new Enemy();
		enemy3->Initialize(enemyModel_.get(), { 28, 1,  40 });
		enemy3->UpdateTransform();
		enemys_.push_back(enemy3);

		Enemy* enemy4 = new Enemy();
		enemy4->Initialize(enemyModel_.get(), { 26, 1.6f,  37 });
		enemy4->UpdateTransform();
		enemys_.push_back(enemy4);
	}

	if (cameraEyeT > cameraSegmentCount * 1100.0f &&
		cameraEyeT < cameraSegmentCount * 1101.0f) {
		enemys_.remove_if([](Enemy* enemy) {
			if (!enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
			});
		Enemy* enemy1 = new Enemy();
		enemy1->Initialize(enemyModel_.get(), { 33, -2,  51 });
		enemy1->UpdateTransform();
		enemys_.push_back(enemy1);

		Enemy* enemy2 = new Enemy();
		enemy2->Initialize(enemyModel_.get(), { 31, 2,  53 });
		enemy2->UpdateTransform();
		enemys_.push_back(enemy2);

		Enemy* enemy3 = new Enemy();
		enemy3->Initialize(enemyModel_.get(), { 35, 1,  54 });
		enemy3->UpdateTransform();
		enemys_.push_back(enemy3);

		Enemy* enemy4 = new Enemy();
		enemy4->Initialize(enemyModel_.get(), { 27, 0,  43 });
		enemy4->UpdateTransform();
		enemys_.push_back(enemy4);

		Enemy* enemy5 = new Enemy();
		enemy5->Initialize(enemyModel_.get(), { 33, 1,  52 });
		enemy5->UpdateTransform();
		enemys_.push_back(enemy5);

		Enemy* enemy6 = new Enemy();
		enemy6->Initialize(enemyModel_.get(), { 28, 1.3f,  47 });
		enemy6->UpdateTransform();
		enemys_.push_back(enemy6);

		Enemy* enemy7 = new Enemy();
		enemy7->Initialize(enemyModel_.get(), { 34, 1.8f,  52 });
		enemy7->UpdateTransform();
		enemys_.push_back(enemy7);
	}
}

void GameScene::RailCustom() {
#ifdef _DEBUG
	int32_t i = 0;
	ImGui::Begin("Rail");
	ImGui::Text("ReDraw : P");
	for (Vector3& pos : controlPoints_) {
		i++;
		/*ImGui::Text("%d.", i);
		ImGui::SameLine();*/
		std::string label = "controlPoint." + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &pos.x, 0.1f);
	}
	if (ImGui::Button("addControlPoint")) {
		Vector3 pos = controlPoints_.back();
		controlPoints_.push_back(pos);
		segmentCount = oneSegmentCount * controlPoints_.size();
		SetSegment();
		RailLineReDraw();
	}
	ImGui::End();
#endif // _DEBUG
}

void GameScene::RailLineReDraw() {
	pointsDrawing_.clear();
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	rails_.remove_if([](Rail* enemy) {
		if (!enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
		});
	for (Vector3& pos : controlPoints_) {
		PopRail(pos, {0,0,0});
	}
}

void GameScene::RailReDraw() {
	rails_.remove_if([](Rail* enemy) {
		if (!enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
		});
	size_t i = 0;
	for (Vector3& v : pointsDrawing_) {
		if (pointsDrawing_[i] == pointsDrawing_.back()) {
			break;
		}
		++i;
		Vector3 rotateRail{};
		Vector3 forward = pointsDrawing_[i] - v;
		// Y軸周り角度(θy)
		rotateRail.y = std::atan2(forward.x, forward.z);
		float length = Length({ forward.x, 0, forward.z });
		// X軸周り角度(θx)
		rotateRail.x = std::atan2(-forward.y, length);
		PopRail(v, rotateRail);
	}
}

void GameScene::RailCameraMove() {
	if (cameraForwardT <= 1.0f) {
		cameraEyeT += cameraSegmentCount;
		cameraForwardT += cameraSegmentCount;
		Vector3 eye = CatmullRomPosition(controlPoints_, cameraEyeT);
		eye.y += 0.5f;
		railCamera_->SetTranslate(eye);
		Vector3 forward = CatmullRomPosition(controlPoints_, cameraForwardT);
		forward.y += 0.5f;
		forward = forward - eye;
		if (cameraForwardT <= 1.0f) {
			Vector3 rotateCametra{};
			// Y軸周り角度(θy)
			rotateCametra.y = std::atan2(forward.x, forward.z);
			float length = Length({ forward.x, 0, forward.z });
			// X軸周り角度(θx)
			rotateCametra.x = std::atan2(-forward.y, length);
			railCamera_->SetRotate(rotateCametra);
		}
	}
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	ImGui::Text("eye%.03f", cameraEyeT);
	ImGui::Text("forward%.03f", cameraForwardT);
	ImGui::End();
#endif // _DEBUG
}

void GameScene::RailCameraDebug() {
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	if (ImGui::Button("StartCamera")) {
		isRailCameraMove_ = true;
	}
	if (ImGui::Button("StopCamera")) {
		isRailCameraMove_ = false;
	}
	if (ImGui::Button("ResetCamera")) {
		ResetRailCamera();
		RailCameraMove();
	}
	if (isRailCameraMove_) {
		RailCameraMove();
	}
	ImGui::End();
#endif // _DEBUG
}

void GameScene::SetSegment() {
	float segmentDenominator = kDivisionSpan * controlPoints_.size();
	cameraSegmentCount = 1.0f / segmentDenominator;
}

void GameScene::ResetRailCamera() {
	float segmentDenominator = kDivisionSpan * controlPoints_.size();
	cameraEyeT = 0;
	cameraForwardT = 30.0f / segmentDenominator;
}

void GameScene::Collision()
{
	int i = 0;
	for (Enemy* enemy : enemys_) {
		Vector3 pos = enemy->GetWorldPosition();
		Matrix4x4 matView = MakeViewportMatrix(0, 0, WindowsApp::kClientWidth, WindowsApp::kClientHieght, 0, 1);
		Matrix4x4 matVPV = railCamera_->GetViewMatrix() * railCamera_->GetProjectionMatrix() * matView;
		pos = TransformM(pos, matVPV);
		Vector2 mouse = input_->GetMousePosition();
#ifdef _DEBUG
		char a[2];
		_itoa_s(i, a, 10);
		ImGui::Begin("enemy");
		ImGui::InputFloat3(a, &pos.x);
		ImGui::End();
#endif // _DEBUG

		if (Length(Vector2{ pos.x, pos.y } - Vector2{ mouse.x, mouse.y }) <= 50.0f && !enemy->IsDead()) {
			enemy->IsCollision();
			score_ += increaseScore_;
			scoreDraw_->SetScore(score_);
		}
		i++;
	}
}
#pragma endregion
