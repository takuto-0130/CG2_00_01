#include "Ground.h"
#include "ModelManager.h"
#include <imgui.h>
#include <numbers>

void Ground::Initialize()
{
	ModelManager::GetInstance()->LoadModel("Resources", "Ground.obj");
	obj_ = new Object3d();
	obj_->Initialize();

	obj_->SetModel("Ground.obj");
	transform_.Initialize();
	transform_.scale_ = { 0.3f,0.3f,0.3f };
	//transform_.rotation_.y = std::numbers::pi_v<float> / 4.0f;
	transform_.translation_.z = -150;
}

void Ground::Update()
{
	transform_.TransferMatrix();
}

void Ground::Draw()
{
	obj_->Draw(transform_);
}
