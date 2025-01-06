#pragma once
#include "Sprite.h"
#include <Object3d.h>
#include "WorldTransform.h"

enum class Status {
	None,
	FadeIn,
	FadeOut
};

class Fade
{
public: // メンバ関数（公開）

	/// <summary>
	/// 初期化
	/// </summary>	
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>		 
	void Update();

	/// <summary>
	/// 描画
	/// </summary>		 
	void Draw();

	void Start(Status status, float duration);

	void Stop();

	bool IsFinished() const;

private:
	void FadeOutUpdate();

	void FadeInUpdate();

private: // メンバ変数（非公開）
	std::unique_ptr<Sprite> sprite_ = nullptr;
	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
};

