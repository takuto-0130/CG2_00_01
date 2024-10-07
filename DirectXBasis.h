#pragma once
#include "WindowsApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Logger.h"
#include "StringUtility.h"
#include <array>
#include <dxcapi.h>

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

	// 描画前処理
	void DrawBegin();

	// 描画後処理
	void DrawEnd();

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
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
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

	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	uint32_t descriptorSizeSRV_ = 0;
	uint32_t descriptorSizeRTV_ = 0;
	uint32_t descriptorSizeDSV_ = 0;
};

