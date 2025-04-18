#pragma once

#include "Object3d.h"
#include "WorldTransform.h"

class BaseObject
{
public:
	virtual ~BaseObject() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;


private:
	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;
};

