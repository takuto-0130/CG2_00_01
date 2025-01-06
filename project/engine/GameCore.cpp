#include "GameCore.h"
#include "ParticleClass.h"
#include "TitleScene.h"
#include "ClearScene.h"
#include "GameScene.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameCore::Initialize()
{
	TYFrameWork::Initialize();

	imgui = ImGuiManager::GetInstance();
	imgui->Initialize(windowsApp.get(), directXBasis.get());

	spriteBasis = SpriteBasis::GetInstance();
	spriteBasis->Initialize(directXBasis.get());

	camera = std::make_unique<Camera>();

	object3dBasis = Object3dBasis::GetInstance();
	object3dBasis->Initialize(directXBasis.get());
	object3dBasis->SetDefaultCamera(camera.get());

	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(directXBasis.get(), srvManager.get());

	Audio::GetInstance()->Initialize();


	sceneArr_[Scene::TITLE] = std::make_unique<TitleScene>();
	sceneArr_[Scene::TITLE]->Init();
	sceneArr_[Scene::TITLE]->SetCamera(camera.get());
	sceneArr_[Scene::STAGE] = std::make_unique<GameScene>();
	sceneArr_[Scene::STAGE]->Init();
	sceneArr_[Scene::STAGE]->SetCamera(camera.get());
	sceneArr_[Scene::CLEAR] = std::make_unique<ClearScene>();
	sceneArr_[Scene::CLEAR]->Init();
	sceneArr_[Scene::CLEAR]->SetCamera(camera.get());
	currentSceneNo_ = 0;
	prevSceneNo_ = currentSceneNo_;
}

void GameCore::Finalize()
{
	TYFrameWork::Finalize();
}

void GameCore::Update()
{
	// Windowsメッセージ処理
	if (windowsApp->ProcessMessage()) {
		// ゲームループを抜ける
		endRequest_ = true;
	}
	else { //ゲーム処理
		TYFrameWork::Update();
		imgui->Begin();
		camera->Update();

		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		if (prevSceneNo_ != currentSceneNo_) {
			sceneArr_[currentSceneNo_]->Init();
		}
		/// ↓更新処理ここから
		sceneArr_[currentSceneNo_]->Update();

		imgui->End();
	}
}

void GameCore::Draw()
{
	///// 描画処理
	directXBasis->DrawBegin();

	srvManager->BeginDraw();

	// 描画コマンド
	//gameScene->Draw();
	sceneArr_[currentSceneNo_]->Draw();

	imgui->Draw();
	directXBasis->DrawEnd();
}
