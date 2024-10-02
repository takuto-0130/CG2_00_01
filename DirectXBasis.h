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
	void Initialize();

	// デバイスの初期化
	void deviceInitialize();

	// コマンド関連の初期化
	void commandInitialize();

	// スワップチェーンの生成
	void swapChainInitialize();

private:
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	WindowsApp* windowsApp = nullptr;
};

