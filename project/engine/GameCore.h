#pragma once
#include "WindowsApp.h"
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#include <fstream>
#include <sstream>
#include <wrl.h>
#include <algorithm>
#include "DirectXBasis.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteBasis.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "Object3dBasis.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
#include "GameScene.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")
class GameCore
{
public: // メンバ変数

	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// 毎フレーム更新
	void Update();

	// 描画
	void Draw();

public:
	// 終了フラグのチェック
	bool IsEndRequest() { return endRequest_; }

private:
	D3DResourceLeakChecker* leakCheck;

	std::unique_ptr<WindowsApp> windowsApp;
	std::unique_ptr<DirectXBasis> directXBasis = nullptr;
	std::unique_ptr<SrvManager> srvManager = nullptr;
	SpriteBasis* spriteBasis = nullptr;
	Object3dBasis* object3dBasis = nullptr;
	ModelManager* modelManager = nullptr;
	Input* input = nullptr;
	ImGuiManager* imgui = nullptr;
	std::unique_ptr<Camera> camera;

	std::unique_ptr<GameScene> gameScene;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;

	bool endRequest_ = false;
};

