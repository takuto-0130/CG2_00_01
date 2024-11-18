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
	void Initialize(std::string textureFilePath);

	// 更新
	void Update();

	// 描画
	void Draw();

	void DrawRect(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb);

	// getter
	const Vector2& GetPosition() { return position_; }
	float GetRotation() const { return rotation_; }
	const Vector4& GetColor() { return materialData_->color; }
	const Vector2& GetSize() { return size_; }
	const Vector2& GetTextureLeftTop() { return textureLeftTop_; }
	const Vector2& GetTextureSize() { return textureSize_; }
	const Vector2& GetAnchorPoint() const { return anchorPoint_; }
	const bool& GetIsFlipX() const { return isFlipX_; }
	const bool& GetIsFlipY() const { return isFlipY_; }

	// setter
	void SetPosition(const Vector2& position) { position_ = position; }
	void SetRotation(const float& rotation) { rotation_ = rotation; }
	void SetColor(const Vector4& color) { materialData_->color = color; }
	void SetSize(const Vector2& size) { size_ = size; }
	void SetTextureLeftTop(Vector2 textureLeftTop) { textureLeftTop_ = textureLeftTop; }
	void SetTextureSize(Vector2 textureSize) { textureSize_ = textureSize; }
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	void SetVertex(const Vector2& position, int index) { vertexData_[index].position = { position.x, position.y, 0.0f,1.0f }; }

	void SetSpriteData();
private: // メンバ関数
	// 頂点データ作成
	void CreateVertexData();

	// マテリアルリソース作成
	void CreateMaterialResource();

	// 座標変換行列リソース作成
	void CreateTransformationMatrixResource();


	void AdjustTextureSize();

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
		Matrix4x4 WorldInverseTranspose;
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
	TransfomationMatrix* transformationMatrixData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	// テクスチャ番号
	uint32_t textureIndex = 0;
	std::string textureFilePath_;

	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	Transform transform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector2 position_ = { 0.0f, 0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 100.0f,100.0f };

	Vector2 anchorPoint_ = { 0.0f, 0.0f };

	Vector2 textureLeftTop_ = { 0.0f, 0.0f };
	Vector2 textureSize_ = { 100.0f, 100.0f };
};

