#pragma once
#include "Object3d.h"
#include "Model.h"
#include "mathFunc.h"
#include "Matrix4x4Func.h"
#include "Weapon.h"
#include "WorldTransform.h"
#include <memory>
#include <Input.h>
#include <numbers>
#include "../global/Collider.h"
#include "GlobalVariables.h"

/// <summary>
/// 振る舞い
/// </summary>
enum class Behavior
{
	kRoot,
	kAttack,
};


class Player : public Collider
{
public: // 構造体
	/// <summary>
	/// コンボ構造体
	/// </summary>
	struct ConstAttack 
	{
		uint32_t anticipationTime;   // 振りかぶり時間
		uint32_t chargeTime;         // ため時間
		uint32_t swingTime;          // 攻撃振り時間
		uint32_t recoveryTime;       // 硬直時間
		float anticipationSpeed;     // 振りかぶり時の移動速度
		float chargeSpeed;           // ため時の移動速度
		float swingSpeed;            // 攻撃時の移動速度
	};

	struct WorkAttack
	{
		uint32_t attackParameter_ = 0; // コンボ進行を管理するパラメーター
		int32_t comboIndex = 0;        // 現在のコンボ段階（0, 1, 2...）
		int32_t inComboPhase = 0;      // 1段の中でどのフェーズか
		bool comboNext = false;        // 次のコンボに進むかどうか
	};

public: // メンバ関数（公開）

	~Player();

	/// <summary>
	/// 初期化
	/// </summary>	
	void Initialize();

	void AddGlobalVariables();

	void ApplyGlobalVariables();

	void AddComboVariables();

	void ApplyComboVariables();

	/// <summary>
	/// 更新
	/// </summary>		 
	void Update();

	/// <summary>
	/// 描画
	/// </summary>		 
	void Draw();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision([[maybe_unused]] Collider* other) override;

	Vector3 GetCenterPosition() const override;

private: // メンバ関数（非公開）

	/// <summary>
	/// 更新処理の末端での処理
	/// </summary>
	void LastUpdate();

	/// <summary>
	/// 自機の移動
	/// </summary>
	void Move();

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
	/// 攻撃行動初期化
	/// </summary>
	void BehaviorAttackInit();

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate();

	/// <summary>
	/// 自機の浮遊
	/// </summary>
	void UpdateFloating();

	void MoveLimit();

public: // アクセッサ1

	Vector3 GetPosition() { return transform_.translation_; }
	Vector3 GetRotation() { return transform_.rotation_; }
	Vector3 GetWeaponPosition() const{return weapon_->GetTranslation();}
	Weapon* GetWeapon() { return weapon_.get(); }

private: // メンバ変数
	Input* input_ = nullptr;
	GlobalVariables* globalVar_ = nullptr;

	// ポインタ
	std::unique_ptr<Weapon> weapon_;

	// 各オブジェクト
	Object3d* baseObject_ = nullptr;
	Object3d* headObject_ = nullptr;
	Object3d* bodyObject_ = nullptr;
	Object3d* L_arm_Object_ = nullptr;
	Object3d* R_arm_Object_ = nullptr;
	float moveSpeed_ = 0;

	// メインのtransform
	WorldTransform transform_;
	WorldTransform body_transform_;
	WorldTransform head_transform_;
	WorldTransform L_arm_transform_;
	WorldTransform R_arm_transform_;


	std::unique_ptr<Object3d> shadowObj_;
	WorldTransform shadowTransform_{};

	// プレイヤーの浮遊用パラメータ
	float playerFloatingParameter_ = 0; // プレイヤーの浮遊パラメータ
	float playerFloatingAmplitude_ = 0; // プレイヤーの浮遊振幅
	float playerFloatingPeriod_ = 0; // プレイヤーの浮遊周期
	float playerFloatingStep_ = 0;

	// 武器の浮遊用パラメータ
	float weaponFloatingParameter_ = 0; // 武器の浮遊パラメータ
	float weaponFloatingAmplitude_ = 0; // 武器の浮遊振幅
	float weaponFloatingPeriod_ = 0; // 武器の浮遊周期
	float weaponFloatingStep_ = 0;

	// 速度
	Vector3 velocity_ = {};

	// コンボの最大数
	static const int ComboNum = 3;

	std::array<ConstAttack, ComboNum> kConstAttacks_;

	WorkAttack workAttack_;

	float moveLimit_ = 0;

public:

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviortRquest_ = std::nullopt;
};

float easeOutQuart(float t);

