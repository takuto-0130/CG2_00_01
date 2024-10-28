#pragma once
#include "struct.h"
#include <wrl.h>
#include <Windows.h>
#include <array>
#include <d3d12.h>

class SpriteBasis;

// スプライト
class Sprite
{
public: // メンバ関数
	// 初期化
	void Initialize(SpriteBasis* spriteBasis);

	// 更新
	void Update();

	// 描画
	void Draw();

	// getter
	const Vector2& GetPosition() { return position_; }
	float GetRotation() const { return rotation_; }
	const Vector4& GetColor() { return materialData_->color; }
	const Vector2& GetSize() { return size_; }

	// 座標のsetter
	void SetPosition(const Vector2& position) { position_ = position; }
	void SetRotation(const float& rotation) { rotation_ = rotation; }
	void SetColor(const Vector4& color) { materialData_->color = color; }
	void SetSize(const Vector2& size) { size_ = size; }

private: // メンバ関数
	// 頂点データ作成
	void CreateVertexData();

	// マテリアルリソース作成
	void CreateMaterialResource();

	// 座標変換行列リソース作成
	void CreateTransformationMatrixResource();

private: // メンバ関数
	SpriteBasis* spriteBasis_;

	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
		Vector3 normal;
	};
	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		std::array<float, 3> padding;
		Matrix4x4 uvTransform;
	};
	// 座標変換行列データ
	struct TransfomationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransfomationMatrix* transfomationMatrixData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	Vector2 position_ = { 0.0f, 0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 640.0f,360.0f };

};

