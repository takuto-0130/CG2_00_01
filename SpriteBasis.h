#pragma once
#include "DirectXBasis.h"

// スプライト共通部
class SpriteBasis
{
public: // メンバ関数
	// 初期化
	void Initialize(DirectXBasis* directXBasis);

	DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

private: // メンバ関数
	// ルートシグネチャの作成
	void CreateRootSignature();

	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private: // メンバ変数
	DirectXBasis* directXBasis_; 
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};

