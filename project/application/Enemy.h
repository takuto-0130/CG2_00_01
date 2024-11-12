#pragma once
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <assert.h>
class Enemy {
public:

	~Enemy();

	void Initialize(Model* model, Vector3 position);

	void Update();

	void Draw();

	Vector3 GetWorldPosition() const;

	bool IsDead() const { return isDead_; }

	void SetRotate(const Vector3& rota) { worldTransform_.rotation_ = rota; }

	void UpdateTransform() { worldTransform_.TransferMatrix(); }

private:

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> enemy_;
	Vector3 velocity_{};
	bool isDead_ = false;
};

