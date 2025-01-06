#include "Camera.h"
#include "operatorOverload.h"

Camera::Camera()
	: transform_({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} })
	, horizontalFOV_(0.45f)
	, aspectRatio_(float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHieght))
	, nearClip_(0.1f)
	, farClip_(500.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_))
	, worldViewProjectionMatrix_(viewMatrix_ * projectionMatrix_)
{}

void Camera::Update()
{
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_);
	worldViewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::FollowCamera(const Vector3& target)
{
	// カメラの位置を対象の後方に設定
	transform_.rotate = followCameraOffsetRotare_;
	transform_.translate = target + followCameraOffsetPosition_;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
}