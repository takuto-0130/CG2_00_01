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
#include "DirectXBasis.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteBasis.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "Object3dBasis.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

class TYFrameWork
{
public:
	// 仮想デストラクタ
	virtual ~TYFrameWork() = default;

	// 実行
	void run();

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 終了フラグのチェック
	virtual bool IsEndRequest() { return endRequest_; };

protected:
#ifdef _DEBUG
	std::unique_ptr<D3DResourceLeakChecker> leakCheck;
#endif // _DEBUG

	std::unique_ptr<WindowsApp> windowsApp = nullptr;
	std::unique_ptr<DirectXBasis> directXBasis = nullptr;
	std::unique_ptr<SrvManager> srvManager = nullptr;
	SpriteBasis* spriteBasis = nullptr;
	Object3dBasis* object3dBasis = nullptr;
	ModelManager* modelManager = nullptr;
	Input* input = nullptr;
	ImGuiManager* imgui = nullptr;
	std::unique_ptr<Camera> camera = nullptr;

	SceneManager* sceneManager_ = nullptr;
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	bool endRequest_ = false;
};



