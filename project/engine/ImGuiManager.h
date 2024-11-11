#pragma once
#include "WindowsApp.h"
#include "DirectXBasis.h"
#include <memory>
#include <mutex>
class ImGuiManager
{
public:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	// シングルトンインスタンスの取得
	static ImGuiManager* GetInstance();
	// 初期化
	void Initialize(WindowsApp* winApp, DirectXBasis* dxBasis);
	// ImGui受付開始
	void Begin();
	// ImGui受付終了
	void End();
	// 描画
	void Draw();
	// 終了
	void Finalize();
private:
	WindowsApp* winApp_ = nullptr;
	DirectXBasis* dxBasis_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

private: // シングルトンインスタンス
	static std::unique_ptr<ImGuiManager> instance;
	static std::once_flag initInstanceFlag;

	ImGuiManager(ImGuiManager&) = default;
	ImGuiManager& operator=(ImGuiManager&) = default;
};

