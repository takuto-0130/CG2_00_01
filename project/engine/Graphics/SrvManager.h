#pragma once
#include "DirectXBasis.h"
class SrvManager
{
public: // メンバ関数
	// 初期化
	void Initialize(DirectXBasis* dxbasis);
	// 確保
	uint32_t Allocate();
	void BeginDraw();
	// SRVの確保が可能かどうかをチェックする関数
	bool CanAllocate() const;

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	// SRV生成 (テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT MipLevels);
	// SRV生成 (Structured Buffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	void SetGraphicsRootDescriptorTable(UINT PootParameterIndex, uint32_t srvIndex);


public:
	// 最大SRV数
	static const uint32_t kMaxCount;
private:
	DirectXBasis* dxBasis_ = nullptr;

	static inline uint32_t descriptorSize_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	uint32_t useIndex_ = 0;
};

