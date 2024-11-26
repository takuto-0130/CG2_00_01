#include "Object3d.h"
#include "Object3dBasis.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include <numbers>

void Object3d::Initialize()
{
	objectManager_ = Object3dBasis::GetInstance();

	CreateMaterialResource();
	CreateCameraResource();
}

void Object3d::Draw(WorldTransform& worldTransform)
{
	camera_ = objectManager_->GetDefaultCamera();
	cameraData_->worldPosition = camera_->GetTranslate();

	// cameraの場所を指定
	objectManager_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());
	// マテリアルCBufferの場所を指定
	objectManager_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw(worldTransform, camera_);
	}
}

void Object3d::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。今回はMaterial1つ分のサイズを用意する
	materialResource_ = objectManager_->GetDirectXBasis()->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 白を入れる
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = 0;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 10.0f;
}

void Object3d::CreateCameraResource()
{
	// カメラ用のリソースを作る
	cameraResource_ = objectManager_->GetDirectXBasis()->CreateBufferResource(sizeof(CameraForGPU));
	// 書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	// 初期値を入れる
	cameraData_->worldPosition = { 1.0f, 1.0f, 1.0f };
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}
