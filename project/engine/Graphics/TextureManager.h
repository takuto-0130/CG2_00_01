#pragma once
#include <Windows.h>
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#include "SrvManager.h"
#include "DirectXBasis.h"
#include <string>
#include <wrl.h>
#include <unordered_map>
#include <mutex>
class TextureManager
{
public: // メンバ関数
	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	// 初期化
	void Initialize(DirectXBasis* dxBasis, SrvManager* srvManager);


	void LoadTexture(const std::string& filePath);

	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	// メタデータを取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

private: // メンバ変数
	// テクスチャ1枚分のデータ
	struct TextureData {
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas_;

	DirectXBasis* dxBasis_ = nullptr;
	SrvManager* srvManager_ = nullptr;

public:
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop_;

private: // シングルトンパターンを適用
	static std::unique_ptr<TextureManager> instance_;
	static std::once_flag initInstanceFlag_;

	TextureManager(TextureManager&) = default;
	TextureManager& operator=(TextureManager&) = default;
public:
	TextureManager() = default;
	~TextureManager() = default;
};

