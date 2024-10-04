#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Logger.h"
#include "StringUtility.h"
#include "WindowsApp.h"
class DirectXBasis
{
public: // メンバ関数
	// 初期化
	void Initialize(WindowsApp* windowsApp);


private: // メンバ関数
	// デバイスの初期化
	void deviceInitialize();

	// コマンド関連の初期化
	void commandInitialize();

	// スワップチェーンの生成
	void swapChainCreate();

	// 深度バッファの生成
	void DepthBufferCreate();

	// 各種ディスクリプターヒープの生成

private: // メンバ変数
	WindowsApp* windowsApp_ = nullptr;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;


	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
};

