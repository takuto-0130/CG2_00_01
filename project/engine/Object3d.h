#pragma once
#include <fstream>
#include "struct.h"
#include "Matrix4x4Func.h"
#include "mathFunc.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
class Object3dBasis;
class WorldTransform;

class Object3d
{
public: // メンバ関数
	Object3d() = default;
	~Object3d() = default;

	void Initialize();

	void Draw(WorldTransform& worldTransform);

private:

	void CreateMaterialResource();

	void CreateCameraResource();

private: // 構造体

	struct Material {
		Vector4 color;
		bool enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	// カメラ座標
	struct CameraForGPU {
		Vector3 worldPosition;
	};

private: // メンバ変数
	Object3dBasis* objectManager_ = nullptr;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;


	Material* materialData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;


	// コピー禁止
	Object3d(const Object3d&) = delete;
	Object3d& operator=(const Object3d&) = delete;

public:
	// モデル
	void SetModel(Model* model) { model_ = model; }
	void SetModel(const std::string& filePath);

	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// 色
	const Vector4& GetColor() const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

	// Lighting
	const bool& GetIsLighting() const { return materialData_->enableLighting; }
	void SetIsLighting(const bool isLighting) { materialData_->enableLighting = isLighting; }
};

static_assert(!std::is_copy_assignable_v<Object3d>);

