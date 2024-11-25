#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "TextureManager.h"

class Skydome {
public:

	/// 初期化
	void Initialize(Model* model);

	/// 更新
	void Update();

	/// 描画
	void Draw();

private:

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> skydome;

};

