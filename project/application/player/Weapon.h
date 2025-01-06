#pragma once
#include "WorldTransform.h"
#include "Object3d.h"
#include "Model.h"
#include "../global/Collider.h"
#include "../global/ContactRecord.h"


class Weapon : public Collider
{
public:

	~Weapon();

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

public: // アクセッサ

	/// <summary>
	/// 回転
	/// </summary>
	const Vector3& GetRotation() { return transform_.rotation_; }
	void SetRotation(const Vector3& rotation) { transform_.rotation_ = rotation; }

	/// <summary>
	/// 拡大・縮小
	/// </summary>
	const Vector3& GetScale() { return transform_.scale_; }
	void SetScale(const Vector3& scale) { transform_.scale_ = scale; }

	/// <summary>
	/// 移動
	/// </summary>
	const Vector3& GetTranslation() { return transform_.translation_; }
	void SetTranslation(const Vector3& translation) { transform_.translation_ = translation; }

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	const WorldTransform& GetWorldTransform() { return transform_; }
	void SetParent(const WorldTransform& worldTransform) { transform_.parent_ = &worldTransform; }

	Vector3 GetCenterPosition() const override;
	void OnCollision([[maybe_unused]] Collider* other) override;

	/// <summary>
    /// 接触履歴を抹消
    /// </summary>
	void RecordClear();

	bool GetisHit() { return isHit_; }
private:

	// トランスフォーム
	WorldTransform transform_;

	// オブジェクト
	Object3d* object_ = nullptr;

	// 接触記録クラス
	ContactRecord contactRecord_;

	bool isHit_ = false;
	
};

