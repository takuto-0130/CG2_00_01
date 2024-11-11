#pragma once
#include "DirectXBasis.h"

// スプライト共通部
class SpriteBasis
{
public: // メンバ関数
	// シングルトンインスタンスの取得
	static SpriteBasis* GetInstance();

	// 初期化
	void Initialize(DirectXBasis* directXBasis);

	// 共通描画設定
	void BasisDrawSetting();

	DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState_.Get(); }

private: // メンバ関数
	// ルートシグネチャの作成
	void CreateRootSignature();

	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private: // メンバ変数
	DirectXBasis* directXBasis_; 
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

private:
	static std::unique_ptr<SpriteBasis> instance;
	static std::once_flag initInstanceFlag;

	SpriteBasis(SpriteBasis&) = default;
	SpriteBasis& operator=(SpriteBasis&) = default;
public:
	SpriteBasis() = default;
	~SpriteBasis() = default;
};

