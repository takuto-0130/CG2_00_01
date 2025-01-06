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
	transform_.scale_ = { 100, 1, 100 };
}

void Ground::Update()
{
	transform_.TransferMatrix();
}

void Ground::Draw()
{
	obj_->Draw(transform_);
}
