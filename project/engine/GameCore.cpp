#include "GameCore.h"
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

	gameScene = std::make_unique<GameScene>();
	gameScene->Initialize(camera.get());
}

void GameCore::Finalize()
{
	//SoundUnload(&soundData1);
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
		gameScene->Update();

		imgui->End();
	}
}

void GameCore::Draw()
{
	///// 描画処理
	directXBasis->DrawBegin();

	srvManager->BeginDraw();

	// 描画コマンド
	gameScene->Draw();

	imgui->Draw();
	directXBasis->DrawEnd();
}
