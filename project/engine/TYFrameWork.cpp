#include "TYFrameWork.h"

void TYFrameWork::run()
{
	Initialize();
	//メインループ
	while (true)
	{
		// 毎フレーム更新処理
		Update();
		// 終了リクエストでループを抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}

void TYFrameWork::Initialize()
{
#pragma region // 基盤システム初期化

	// ウィンドウズアプリケーション
	windowsApp = std::make_unique<WindowsApp>();
	windowsApp->Initialize();

	directXBasis = std::make_unique<DirectXBasis>();
	directXBasis->Initialize(windowsApp.get());

	srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(directXBasis.get());

	input = Input::GetInstance();
	input->Initialize(windowsApp->GetHwnd());

	IXAudio2MasteringVoice* masterVoice;
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
	TextureManager::GetInstance()->Initialize(directXBasis.get(), srvManager.get());
#pragma endregion
}

void TYFrameWork::Finalize()
{
	xAudio2.Reset();
	imgui->Finalize();
	input->Finalize();
	windowsApp->Finalize();
}

void TYFrameWork::Update()
{
	input->Update();
}
