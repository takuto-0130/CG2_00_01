#include "Sprite.h"
#include "SpriteBasis.h"
#include "Matrix4x4Func.h"
#include "struct.h"


void Sprite::Initialize(SpriteBasis* spriteBasis)
{
	spriteBasis_ = spriteBasis;
	CreateVertexData();
	CreateMaterialResource();
	CreateTransformationMatrixResource();
}

void Sprite::Update()
{
	vertexData_[0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[0].texCoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texCoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[2].texCoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[3].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texCoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	Transform transform
	{
		.scale{ size_.x, size_.y, 1.0f} ,
		.rotate{ 0.0f, 0.0f, rotation_},
		.translate{ position_.x, position_.y, 0.0f}
	};
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHieght), 0.0f, 100.0f);

	transfomationMatrixData_->WVP = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transfomationMatrixData_->World = worldMatrix;
}

void Sprite::Draw()
{
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	spriteBasis_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

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

	// データを書き込むためのアドレスを取得してそれぞれに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
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
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transfomationMatrixData_));
	// 単位行列を書き込んでおく
	transfomationMatrixData_->WVP = MakeIdentity4x4();
	transfomationMatrixData_->World = MakeIdentity4x4();
}
