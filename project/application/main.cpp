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

#pragma region //サウンド

struct ChunkHeader {
	char id[4];
	int32_t size;
};

struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	unsigned int bufferSize;
	int playSoundLength;
};

SoundData SoundLoadWave(const char* filename) {

	std::ifstream file;
	file.open(filename, std::ios_base::binary);
	assert(file.is_open());

	RiffHeader riff;
	file.read((char*)&riff.chunk.id, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	FormatChunk format = {};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {//資料では!=0
		assert(0);
	}
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	file.close();

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.playSoundLength = data.size / format.fmt.nBlockAlign;

	return soundData;
}


void SoundUnload(SoundData* soundData) {
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}


void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData, bool isLoop = false, float volume = 1.0f) {
	HRESULT hr;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.PlayBegin = 0;
	buf.PlayLength = soundData.playSoundLength;

	if(isLoop){
		buf.LoopBegin = 0;
		buf.LoopLength = soundData.playSoundLength;
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	volume = std::clamp(volume, 0.0f, 1.0f);
	hr = pSourceVoice->SetVolume(volume);
	hr = pSourceVoice->Start();
}

#pragma endregion


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	D3DResourceLeakChecker* leakCheck;
	(void)leakCheck;
#pragma region // 基盤システム初期化
	WindowsApp* windowsApp = nullptr;
	DirectXBasis* directXBasis = nullptr;
	SrvManager* srvManager = nullptr;
	SpriteBasis* spriteBasis = nullptr;
	Object3dBasis* object3dBasis = nullptr;
	ModelManager* modelManager = nullptr;
	Input* input = nullptr;
	std::unique_ptr<Camera> camera;
	ImGuiManager* imgui = nullptr;

	windowsApp = new WindowsApp();
	windowsApp->Initialize();

	directXBasis = new DirectXBasis;
	directXBasis->Initialize(windowsApp);

	srvManager = new SrvManager();
	srvManager->Initialize(directXBasis);
	
	spriteBasis = SpriteBasis::GetInstance();
	spriteBasis->Initialize(directXBasis);

	camera = std::make_unique<Camera>();

	object3dBasis = Object3dBasis::GetInstance();
	object3dBasis->Initialize(directXBasis);
	object3dBasis->SetDefaultCamera(camera.get());

	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(directXBasis, srvManager);
	
	input = Input::GetInstance();
	input->Initialize(windowsApp->GetHwnd());

	imgui = ImGuiManager::GetInstance();
	imgui->Initialize(windowsApp, directXBasis);

#pragma endregion

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	TextureManager::GetInstance()->Initialize(directXBasis, srvManager);

	std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
	gameScene->Initialize(camera.get());

	/*SoundData soundData1 = SoundLoadWave("Resources/fanfare.wav");

	SoundPlayWave(xAudio2.Get(), soundData1, 0.3f);*/

	//メインループ
	while (true) 
	{
		// Windowsメッセージ処理
		if (windowsApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		else { //ゲーム処理
			imgui->Begin();
			input->Update();
			camera->Update();

			gameScene->Update();

			imgui->End();
			///// 描画処理
			directXBasis->DrawBegin();

			srvManager->BeginDraw();
			
			// 描画コマンド
			gameScene->Draw();

			imgui->Draw();
			directXBasis->DrawEnd();
			
		}
	}
	xAudio2.Reset();
	//SoundUnload(&soundData1);
	imgui->Finalize();
	delete srvManager;
	delete directXBasis;
	windowsApp->Finalize();
	delete windowsApp;

	return 0;
}