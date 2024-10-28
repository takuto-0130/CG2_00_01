#include "WindowsApp.h"
//#include <d3d12.h>
//#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <sstream>
#include <wrl.h>
#include <algorithm>
#include "Input.h"
#include "DirectXBasis.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteBasis.h"
#include "Sprite.h"
#include "struct.h"
#include "Matrix4x4Func.h"
#include "TextureManager.h"


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

//////////
// 関数 //
//////////
#pragma region // 関数



//Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const int32_t& width, const int32_t& height) {
//	//生成するResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = width;
//	resourceDesc.Height = height;
//	resourceDesc.MipLevels = 1;
//	resourceDesc.DepthOrArraySize = 1;
//	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
//	resourceDesc.SampleDesc.Count = 1;
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知
//
//	//利用するHeapの設定
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る
//
//	//深度のクリア設定
//	D3D12_CLEAR_VALUE depthClearValue{};
//	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
//	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる
//
//	//Resourceの生成
//	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,
//		D3D12_HEAP_FLAG_NONE,
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度情報を書き込む状態にしておく
//		&depthClearValue,
//		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
//	assert(SUCCEEDED(hr));
//	return resource;
//}
#pragma endregion

#pragma region // 構造体

float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	Vector3 result{ v.x / len, v.y / len, v.z / len };
	return result;
}
#pragma endregion

MateerialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MateerialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifiler;
		std::istringstream s(line);
		s >> identifiler;

		if (identifiler == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;//ファイルから読んだ1行を保存

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifiler;
		std::istringstream s(line);
		s >> identifiler;

		if (identifiler == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifiler == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifiler == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifiler == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifiler == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	D3DResourceLeakChecker* leakCheck;
#pragma region // 基盤システム初期化
	WindowsApp* windowsApp = nullptr;
	DirectXBasis* directXBasis = nullptr;
	SpriteBasis* spriteBasis = nullptr;
	Input* input = nullptr;

	windowsApp = new WindowsApp();
	windowsApp->Initialize();

	directXBasis = new DirectXBasis;
	directXBasis->Initialize(windowsApp);
	
	spriteBasis = new SpriteBasis();
	spriteBasis->Initialize(directXBasis);

	input = new Input();
	input->Initialize(windowsApp);

#pragma endregion

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	/*SoundData soundData1 = SoundLoadWave("Resources/fanfare.wav");

	SoundPlayWave(xAudio2.Get(), soundData1, 0.3f);*/
	uint32_t spriteNum = 5;
	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < spriteNum; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteBasis);
		sprites.push_back(sprite);
	}


	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteBasis);

	uint32_t itr = 0;
	for (Sprite* spr : sprites) {
		spr->SetSize({ 50.0f,50.0f });
		spr->SetPosition({ 100.0f * static_cast<float>(itr), 0.0f });
		itr++;
	}

	TextureManager::GetInstance()->Initialize();



	const uint32_t kSubdivision = 16;
	//実際に頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = directXBasis->CreateBufferResource(sizeof(VertexData) * 6 * kSubdivision * kSubdivision);

	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭アドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースサイズは頂点3つ分のサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 6 * kSubdivision * kSubdivision;
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//////左下
	////vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	////vertexData[0].texCoord = { 0.0f,1.0f };
	//////上
	////vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	////vertexData[1].texCoord = { 0.5f,0.0f };
	//////右下
	////vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	////vertexData[2].texCoord = { 1.0f,1.0f };
	//////左下2
	////vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	////vertexData[3].texCoord = { 0.0f,1.0f };
	//////上2
	////vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	////vertexData[4].texCoord = { 0.5f,0.0f };
	//////右下2
	////vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	////vertexData[5].texCoord = { 1.0f,1.0f };


	const float kLonEvery = float(M_PI) * 2.0f / kSubdivision;
	const float kLatEvery = float(M_PI) / kSubdivision;
	const float kUVEvery = 1.0f / float(kSubdivision);
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	//	float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
	//		float lon = lonIndex * kLonEvery;
	//		float u = float(lonIndex) / float(kSubdivision);
	//		float v = 1.0f - float(latIndex) / float(kSubdivision);
	//		// A
	//		vertexData[start].position = { cosf(lat) * cosf(lon), sinf(lat), cosf(lat) * sinf(lon), 1.0f };
	//		vertexData[start].texCoord = { u,v };
	//		vertexData[start].normal = { vertexData[start].position.x, vertexData[start].position.y, vertexData[start].position.z };
	//		// B
	//		vertexData[start + 1].position = { cosf(lat + kLatEvery) * cosf(lon), sinf(lat + kLatEvery), cosf(lat + kLatEvery) * sinf(lon), 1.0f };
	//		vertexData[start + 1].texCoord = { u,v - kUVEvery };
	//		vertexData[start + 1].normal = { vertexData[start + 1].position.x, vertexData[start + 1].position.y, vertexData[start + 1].position.z };
	//		// C
	//		vertexData[start + 2].position = { cosf(lat) * cosf(lon + kLonEvery), sinf(lat), cosf(lat) * sinf(lon + kLonEvery), 1.0f };
	//		vertexData[start + 2].texCoord = { u + kUVEvery,v };
	//		vertexData[start + 2].normal = { vertexData[start + 2].position.x, vertexData[start + 2].position.y, vertexData[start + 2].position.z };
	//		// B:Copy
	//		vertexData[start + 3].position = vertexData[start + 1].position;
	//		vertexData[start + 3].texCoord = vertexData[start + 1].texCoord;
	//		vertexData[start + 3].normal = { vertexData[start + 3].position.x, vertexData[start + 3].position.y, vertexData[start + 3].position.z };
	//		// D
	//		vertexData[start + 4].position = { cosf(lat + kLatEvery) * cosf(lon + kLonEvery), sinf(lat + kLatEvery), cosf(lat + kLatEvery) * sinf(lon + kLonEvery), 1.0f };
	//		vertexData[start + 4].texCoord = { u + kUVEvery,v - kUVEvery };
	//		vertexData[start + 4].normal = { vertexData[start + 4].position.x, vertexData[start + 4].position.y, vertexData[start + 4].position.z };
	//		// C:Copy
	//		vertexData[start + 5].position = vertexData[start + 2].position;
	//		vertexData[start + 5].texCoord = vertexData[start + 2].texCoord;
	//		vertexData[start + 5].normal = { vertexData[start + 5].position.x, vertexData[start + 5].position.y, vertexData[start + 5].position.z };
	//	}
	//}


	//==================================
	//modelData用
	//==================================
	//ModelData modelData = LoadObjFile("Resources", "teapot.obj");

	/*Microsoft::WRL::ComPtr<ID3D12Resource> modelVertexResource = directXBasis->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	D3D12_VERTEX_BUFFER_VIEW modelVertexBufferView{};
	modelVertexBufferView.BufferLocation = modelVertexResource->GetGPUVirtualAddress();
	modelVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	modelVertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* modelVertexData = nullptr;
	modelVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&modelVertexData));
	std::memcpy(modelVertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	ModelData modelData2 = LoadObjFile("Resources", "bunny.obj");

	Microsoft::WRL::ComPtr<ID3D12Resource> modelVertexResource2 = directXBasis->CreateBufferResource(sizeof(VertexData) * modelData2.vertices.size());

	D3D12_VERTEX_BUFFER_VIEW modelVertexBufferView2{};
	modelVertexBufferView2.BufferLocation = modelVertexResource2->GetGPUVirtualAddress();
	modelVertexBufferView2.SizeInBytes = UINT(sizeof(VertexData) * modelData2.vertices.size());
	modelVertexBufferView2.StrideInBytes = sizeof(VertexData);

	VertexData* modelVertexData2 = nullptr;
	modelVertexResource2->Map(0, nullptr, reinterpret_cast<void**>(&modelVertexData2));
	std::memcpy(modelVertexData2, modelData2.vertices.data(), sizeof(VertexData) * modelData2.vertices.size());*/




	//==================================
	//Sprite用
	//==================================
	







	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = directXBasis->CreateBufferResource(sizeof(Material));
	////マテリアルにデータを書き込む
	//Material* materialData = nullptr;
	////書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	////色
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLighting = 2;
	//materialData->uvTransform = MakeIdentity4x4();



	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource = directXBasis->CreateBufferResource(sizeof(TransfomationMatrix));

	//TransfomationMatrix* transformationMatrixData = nullptr;

	//transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//Matrix4x4 viewMatrix = MakeIdentity4x4();
	//Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	//*transformationMatrixData =
	//{
	//	worldViewProjectionMatrix,
	//	MakeIdentity4x4()
	//};



	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource2 = directXBasis->CreateBufferResource(sizeof(TransfomationMatrix));

	//TransfomationMatrix* transformationMatrixData2 = nullptr; 
	//
	//Transform transform2{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{-2.0f,0.0f,0.0f} };

	//Matrix4x4 worldMatrix2 = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);
	//Matrix4x4 viewMatrix2 = MakeIdentity4x4();
	//Matrix4x4 projectionMatrix2 = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrix2 = Multiply(worldMatrix2, Multiply(viewMatrix2, projectionMatrix2));

	//transformationMatrixResource2->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData2));
	//*transformationMatrixData2 =
	//{
	//	worldViewProjectionMatrix2,
	//	MakeIdentity4x4()
	//};



	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource3 = directXBasis->CreateBufferResource(sizeof(TransfomationMatrix));

	//TransfomationMatrix* transformationMatrixData3 = nullptr;

	//Transform transform3{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{2.0f,0.0f,0.0f} };

	//Matrix4x4 worldMatrix3 = MakeAffineMatrix(transform3.scale, transform3.rotate, transform3.translate);
	//Matrix4x4 viewMatrix3 = MakeIdentity4x4();
	//Matrix4x4 projectionMatrix3 = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrix3 = Multiply(worldMatrix3, Multiply(viewMatrix3, projectionMatrix3));

	//transformationMatrixResource3->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData3));
	//*transformationMatrixData3 =
	//{
	//	worldViewProjectionMatrix3,
	//	MakeIdentity4x4()
	//};




	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = directXBasis->CreateBufferResource(sizeof(DirectionalLight));

	DirectionalLight* directionalLightData = nullptr;

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;




	//transform変数を作る
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	DirectX::ScratchImage mipImages = directXBasis->LoadTexture("./Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = directXBasis->CreateTextureResource(metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = directXBasis->UploadTextureData(textureResource, mipImages);

	/*DirectX::ScratchImage mipImages2 = directXBasis->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = directXBasis->CreateTextureResource(metadata2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = directXBasis->UploadTextureData(textureResource2, mipImages2);*/




	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////metaDataを元にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	////SRVを作成するDescriptorHeapの場所を決める
	////先頭はImGuiが使っているのでその次使う
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = directXBasis->GetSRVCpuDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = directXBasis->GetSRVGpuDescriptorHandle(1);
	////SRVの作成
	directXBasis->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = directXBasis->GetSRVCpuDescriptorHandle(2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = directXBasis->GetSRVGpuDescriptorHandle(2);
	////SRVの作成
	//directXBasis->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);


	bool useMonsterBall = true;

	Microsoft::WRL::ComPtr<ID3D12Device> device = directXBasis->GetDevice();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = directXBasis->GetCommandList();

	//メインループ
	while (true) 
	{
		// Windowsメッセージ処理
		if (windowsApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		else { //ゲーム処理
			input->Update();
			if (input->PushKey(DIK_0)) {
				OutputDebugStringA("Hit 0\n");
			}
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Window");

			/*ImGui::Combo("Lighting", &materialData->enableLighting, "None\0Lambert\0Harf Lambert\0");
			ImGui::DragFloat3("materialColor", &materialData->color.x, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat3("Lightdirection", &directionalLightData->direction.x, 0.05f, -1.0f, 1.0f);
			ImGui::DragFloat3("LightColor", &directionalLightData->color.x, 0.05f, 0.0f, 1.0f);
			ImGui::SliderFloat("LightIntensity", &directionalLightData ->intensity, 0.0f, 1.0f);
			ImGui::Text("\n");
			ImGui::DragFloat3("teaScale", &transform.scale.x, 0.1f, 0.1f, 5.0f);
			ImGui::DragFloat3("teaRotare", &transform.rotate.x, 0.1f);
			ImGui::DragFloat3("teaTranslate", &transform.translate.x, 0.1f);
			ImGui::Text("\n");
			ImGui::DragFloat3("bunnyScale", &transform3.scale.x, 0.1f, 0.1f, 5.0f);
			ImGui::DragFloat3("bunnyRotare", &transform3.rotate.x, 0.1f);
			ImGui::DragFloat3("bunnyTranslate", &transform3.translate.x, 0.1f);
			ImGui::Text("\n");
			ImGui::DragFloat3("SphereScale", &transform2.scale.x, 0.1f, 0.1f, 5.0f);
			ImGui::DragFloat3("SphereRotare", &transform2.rotate.x, 0.1f);
			ImGui::DragFloat3("SphereTranslate", &transform2.translate.x, 0.1f);
			ImGui::Text("\n");*/
			/*ImGui::DragFloat2("SpriteScale", &transformSprite.scale.x, 0.1f, 0.1f, 5.0f);
			ImGui::DragFloat("SpriteRotate", &transformSprite.rotate.z, 0.1f);
			ImGui::DragFloat2("SpriteTranslate", &transformSprite.translate.x, 1.0f);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f);
			ImGui::SliderAngle("UVRotare", &uvTransformSprite.rotate.z);*/
			ImGui::End();
			directionalLightData->direction = Normalize(directionalLightData->direction);

			//directXBasis->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			/*worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			*transformationMatrixDataSprite =
			{
				worldViewProjectionMatrixSprite,
				MakeIdentity4x4()
			};*/

			//transform.rotate.y += 0.01f;

			/*worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			viewMatrix = Inverse(cameraMatrix);
			projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHieght), 0.1f, 100.0f);
			worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			transformationMatrixData->WVP = worldViewProjectionMatrix;
			transformationMatrixData->World = worldMatrix;

			worldMatrix2 = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);
			viewMatrix2 = Inverse(cameraMatrix);
			projectionMatrix2 = MakePrespectiveFovMatrix(0.45f, float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHieght), 0.1f, 100.0f);
			worldViewProjectionMatrix2 = Multiply(worldMatrix2, Multiply(viewMatrix2, projectionMatrix2));
			transformationMatrixData2->WVP = worldViewProjectionMatrix2;
			transformationMatrixData2->World = worldMatrix2;

			worldMatrix3 = MakeAffineMatrix(transform3.scale, transform3.rotate, transform3.translate);
			viewMatrix3 = Inverse(cameraMatrix);
			projectionMatrix3 = MakePrespectiveFovMatrix(0.45f, float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHieght), 0.1f, 100.0f);
			worldViewProjectionMatrix3 = Multiply(worldMatrix3, Multiply(viewMatrix3, projectionMatrix3));
			transformationMatrixData3->WVP = worldViewProjectionMatrix3;
			transformationMatrixData3->World = worldMatrix3;*/


			/*Matrix4x4 uvTransformMatrix = MakeAffineMatrix(uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);
			materialDataSprite->uvTransform = uvTransformMatrix;*/
			for (Sprite* spr : sprites) {
				spr->Update();
			}
			sprite->Update();
			ImGui::Render();

			///// 描画処理

			directXBasis->DrawBegin();

			// 描画コマンド
			spriteBasis->BasisDrawSetting();

			/*commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource2->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			commandList->DrawInstanced(6 * kSubdivision * kSubdivision, 1, 0, 0);

			commandList->IASetVertexBuffers(0, 1, &modelVertexBufferView);
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU2);
			commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			commandList->IASetVertexBuffers(0, 1, &modelVertexBufferView2);
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource3->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			commandList->DrawInstanced(UINT(modelData2.vertices.size()), 1, 0, 0);*/

			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			for (Sprite* spr : sprites) {
				spr->Draw();
			}
			//sprite->Draw();



			//ImGuiコマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

			directXBasis->DrawEnd();
			
		}
	}
	TextureManager::GetInstance()->Finalize();
	xAudio2.Reset();
	//SoundUnload(&soundData1);
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	for (Sprite* spr : sprites) {
		delete spr;
	}
	delete sprite;
	delete input;
	delete spriteBasis;
	delete directXBasis;
	windowsApp->Finalize();
	delete windowsApp;

	return 0;
}