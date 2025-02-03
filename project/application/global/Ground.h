#pragma once
#include "WorldTransform.h"
#include "Object3d.h"
#include "Model.h"



class Ground
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// < summary>
	/// 更新
	/// </summary>
	void Update();

	/// < summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform transform_;
	std::unique_ptr<Object3d> obj_;
};

