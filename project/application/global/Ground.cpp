#include "Ground.h"
#include "ModelManager.h"
#include <imgui.h>
#include <numbers>
#include "GlobalVariables.h"

void Ground::Initialize()
{
	ModelManager::GetInstance()->LoadModel("Resources", "Ground.obj");
	obj_ = new Object3d();
	obj_->Initialize();

	obj_->SetModel("Ground.obj");
	transform_.Initialize();

	transform_.scale_ = GlobalVariables::GetInstance()->GetVector3Value("global", "fieldScale");
}

void Ground::Update()
{
	transform_.TransferMatrix();
}

void Ground::Draw()
{
	obj_->Draw(transform_);
}
