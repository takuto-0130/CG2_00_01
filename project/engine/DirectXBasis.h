#pragma once
#include "WindowsApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <chrono>
#include "Logger.h"
#include "StringUtility.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"

class DirectXBasis
{
public: // メンバ関数
	// 初期化
	void Initialize(WindowsApp* windowsApp);

	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuDescriptorHandle(const uint32_t& index);

	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuDescriptorHandle(const uint32_t& index);

	// シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);
	
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const size_t& sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);


	void ClearDepthStencilView();

	// 描画前処理
	void DrawBegin();

	// 描画後処理
	void DrawEnd();

	void CommandListAndFence();


	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// 描画コマンドキューの取得
	/// </summary>
	/// <returns>描画コマンドキュー</returns>
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

	/// <summary>
	/// 描画コマンドアロケータの取得
	/// </summary>
	/// <returns>描画コマンドアロケータ</returns>
	ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator_.Get(); }

private: // メンバ関数
	// デバイスの初期化
	void InitDevice();

	// コマンド関連の初期化
	void InitCommand();

	// スワップチェーンの生成
	void CreateSwapChain();

	// 深度バッファの生成
	void CreateDepthBuffer();

	// 各種デスクリプターヒープの生成
	void CreateVariousDescriptorHeap();

	// レンダーターゲットビューの初期化
	void InitRTV();

	// 深度ステンシルビューの初期化
	void InitDSV();

	// フェンスの初期化
	void InitFence();

	// ビューポート矩形の初期化
	void InitViewportRect();

	// シザリング矩形の初期化
	void InitScissorRect();

	// DXCコンパイラの生成
	void CreateDXCCompiler();

	// ImGuiの初期化
	void InitImGui();

	// fps固定の初期化
	void InitFixFPS();

	// fps固定の更新
	void UpdateFixFPS();

	// 記録時間 (fps固定用)
	std::chrono::steady_clock::time_point reference_;

public:

	// デスクリプターヒープを生成する
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDeacriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, const bool& shaderVisible);

	/// <summary>
	/// 指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);

	/// <summary>
	/// 指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);

	// バックバッファの数を取得
	size_t GetBackBufferCount() { return backBuffers_.size(); }

public: // メンバ変数
	// 最大SRV数(最大テクスチャ数)
	static const uint32_t kMaxSRVCount_;

private: // メンバ変数
	WindowsApp* windowsApp_ = nullptr;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescripterHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> backBuffers_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint32_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	D3D12_RESOURCE_BARRIER barrier_{};

	D3D12_VIEWPORT viewportRect_{};
	D3D12_RECT scissorRect_{};

	uint32_t descriptorSizeSRV_ = 0;
	uint32_t descriptorSizeRTV_ = 0;
	uint32_t descriptorSizeDSV_ = 0;
};

