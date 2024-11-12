#include "Enemy.h"
#include "mathFunc.h"
#include "operatorOverload.h"



Enemy::~Enemy() {
}


void Enemy::Initialize(Model* model, Vector3 position) {
	assert(model);
	enemy_ = std::make_unique<Object3d>();
	enemy_->Initialize();
	enemy_->SetModel(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.7f, 0.7f, 0.7f };
}

void Enemy::Update() {
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() {
	enemy_->Draw(worldTransform_);
}

Vector3 Enemy::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
