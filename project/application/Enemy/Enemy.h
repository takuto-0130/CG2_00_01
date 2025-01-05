#pragma once
#include "Object3d.h"
#include "mathFunc.h"
#include <WorldTransform.h>
#include "../global/Collider.h"
#include "../player/Weapon.h"

class Player;
/// <summary>
/// 振る舞い
/// </summary>
enum class EnemyBehavior {
	kRoot,
	kKnockBack,
	kCorpse,
	kLight
};
class Enemy : public Collider
{
public: // メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// プレイヤーのセッター
	/// </summary>
	void SetPlayer(Player* player) { player_ = player; }

	bool IsDelete() const { return isDelete_; }

	/// <summary>
	/// 当たり判定
	/// </summary>
	void OnCollision([[maybe_unused]] Collider* other) override;

	Vector3 GetCenterPosition() const override;

	/// <summary>
	/// シリアルナンバーの取得
	/// </summary>
	uint32_t GetSerialNumber() const { return serialNumber_; }

	const WorldTransform& GetWorldTransform() { return transform_; }


	void SetOnLight(bool onLight) { onLight_ = onLight; }

	bool GetOnLight() const { return onLight_; }

	Vector3 GetPosition() { return transform_.translation_; }


private: // メンバ関数
	/// <summary>
	/// 自機の移動
	/// </summary>
	void Move();

	/// <summary>
	/// 敵を自機に向ける
	/// </summary>
	void Turning();

	/// <summary>
	/// SRT更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// モーション全体の初期化
	/// </summary>
	void BehaviorInitialize();

	/// <summary>
	/// モーション全体の更新
	/// </summary>
	void BehaviorUpdate();

	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void BehaviorRootInit();

	/// <summary>
	/// ノックバック初期化
	/// </summary>
	void BehaviorKnockBackInit();

	/// <summary>
	/// 死んでるとき初期化
	/// </summary>
	void BehaviorCorpseInit();

	/// <summary>
	/// ライト初期化
	/// </summary>
	void BehaviorLightInit();


	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// ノックバック更新
	/// </summary>
	void BehaviorKnockBackUpdate();

	/// <summary>
	/// 死んでるとき更新
	/// </summary>
	void BehaviorCorpseUpdate();

	/// <summary>
	/// ライト更新
	/// </summary>
	void BehaviorLightUpdate();


	float easeOutBounce(float x);

private: // メンバ変数
	std::unique_ptr<Object3d> object_;
	WorldTransform transform_{};

	// プレイヤーの情報取得用
	Player* player_ = nullptr;
#pragma region // エネミーのステータス(外部に出す)
	const float kNomalSpeed_ = 0.4f;
	const float kOnLightSpeed_ = 0.1f;
	float speed_ = kNomalSpeed_;
	float radius_ = 0.3f;
	bool isDelete_ = false;

	float paramater_ = 0;
	float step_ = 1.0f;
	float knockBackTime_ = 60.0f;
	float startY_ = 0;

	
	bool onCollision_ = false;
	// シリアルナンバー
	uint32_t serialNumber_ = 0;
	// 次のシリアルナンバー
	static uint32_t nextSerialNumber_;


	const float kKnockBackSpeed_ = 0.08f;
	Vector3 knockBackVec_{};

	const int kMaxCorpseTimer_ = 120;
	int corpseTimer_ = 0;

	bool onLight_ = false;
	const int kMaxLightTimer_ = 360;
	int lightTimer_ = 0;
#pragma endregion

public:
	// 振る舞い
	EnemyBehavior behavior_ = EnemyBehavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<EnemyBehavior> behaviortRquest_ = std::nullopt;
};

