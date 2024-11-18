#include "Sprite.h"
#include "SpriteBasis.h"
#include "Matrix4x4Func.h"
#include "struct.h"
#include "TextureManager.h"


void Sprite::Initialize(std::string textureFilePath)
{
	spriteBasis_ = SpriteBasis::GetInstance();
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
	textureFilePath_ = textureFilePath;

	CreateVertexData();
	CreateMaterialResource();
	CreateTransformationMatrixResource();
	AdjustTextureSize();
}

void Sprite::Update()
{
	SetSpriteData();

	transform_.translate = { position_.x, position_.y, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, rotation_ };
	transform_.scale = { size_.x, size_.y, 1.0f };

	// Transform情報を作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData_->World = worldViewProjectionMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
}

void Sprite::Draw()
{
	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));
	//// 描画 (インスタンスについては今後)
	spriteBasis_->GetDirectXBasis()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::DrawRect(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb)
{
	// 左下
	vertexData_[0].position = { lb.x, lb.y, 0.0f, 1.0f };
	// 左上
	vertexData_[1].position = { lt.x, lt.y, 0.0f, 1.0f };
	// 右下
	vertexData_[2].position = { rb.x, rb.y, 0.0f, 1.0f };
	// 右上
	vertexData_[3].position = { rt.x, rt.y, 0.0f, 1.0f }; 

	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	transform_.translate = { 0, 0, 0 };
	transform_.rotate = { 0, 0, 0 };
	transform_.scale = { 1.0f,  1.0f, 1.0f };

	// Transform情報を作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData_->World = worldViewProjectionMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;

	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));
	spriteBasis_->GetDirectXBasis()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateVertexData()
{
	// リソースの作成
	vertexResource_ = spriteBasis_->GetDirectXBasis()->CreateBufferResource(sizeof(VertexData) * 4);
	indexResource_ = spriteBasis_->GetDirectXBasis()->CreateBufferResource(sizeof(uint32_t) * 6);

	// バッファビューの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4; // 使用するリソースのサイズは頂点データ4つ分のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData); // 1頂点当たりのサイズ

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6; // 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT; // インデックスはuint32_tとする
}

void Sprite::CreateMaterialResource()
{
	materialResource_ = spriteBasis_->GetDirectXBasis()->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixResource()
{
	transformationMatrixResource_ = spriteBasis_->GetDirectXBasis()->CreateBufferResource(sizeof(TransfomationMatrix));
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書き込んでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WorldInverseTranspose = MakeIdentity4x4();
}

void Sprite::SetSpriteData()
{
	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	float tex_left = textureLeftTop_.x / metadata.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	float tex_top = textureLeftTop_.y / metadata.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// スプライトの描画
	// 左下
	vertexData_[0].position = { left, bottom, 0.0f, 1.0f };
	vertexData_[0].texCoord = { tex_left, tex_bottom };
	vertexData_[0].normal = { 0.0f, 0.0f, -1.0f };
	// 左上
	vertexData_[1].position = { left, top, 0.0f, 1.0f };
	vertexData_[1].texCoord = { tex_left, tex_top };
	vertexData_[1].normal = { 0.0f, 0.0f, -1.0f };
	// 右下
	vertexData_[2].position = { right, bottom, 0.0f, 1.0f };
	vertexData_[2].texCoord = { tex_right, tex_bottom };
	vertexData_[2].normal = { 0.0f, 0.0f, -1.0f };
	// 右上
	vertexData_[3].position = { right, top, 0.0f, 1.0f };
	vertexData_[3].texCoord = { tex_right, tex_top };
	vertexData_[3].normal = { 0.0f, 0.0f, -1.0f };

	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;
}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}
