#include "Collider.h"

void Collider::Initialize() {
	worldTransform_.Initialize();
}

void Collider::UpdateWorldTransform() {
	// ワールド座標をワールドトランスフォームに適用
	worldTransform_.translation_ = GetCenterPosition();
	// ワールドトランスフォームを再計算して定数バッファに書き込む
	worldTransform_.TransferMatrix();
}

void Collider::Draw(Object3d* obj) {
	obj->Draw(worldTransform_);
}


