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

/// <summary>
/// 振る舞い
/// </summary>
enum class Behavior {
	kRoot,
	kAttack,
	kJump
};

enum class ComboPhase {
	kAnticipation,	// 振りかぶり
	kCharge,		// ため
	kSwing,			// 攻撃の振り
	kRecovery		// 攻撃後の硬直
};

class Player : public Collider
{
public: // 構造体
	/// <summary>
	/// コンボ構造体
	/// </summary>
	struct ConstAttack {
		uint32_t anticipationTime;   // 振りかぶりの時間<frame>
		uint32_t chargeTime;         // ための時間<frame>
		uint32_t swingTime;          // 攻撃の振りの時間<frame>
		uint32_t recoveryTime;       // 攻撃後の硬直時間<frame>
		float anticipationSpeed;     // 振りかぶり時の移動速度
		float chargeSpeed;           // ため時の移動速度
		float swingSpeed;            // 攻撃時の移動速度
	};
	struct WorkAttack {
		uint32_t attackParameter_ = 0; // コンボ進行を管理するパラメーター
		int32_t comboIndex = 0;        // 現在のコンボ段階（0, 1, 2...）
		int32_t inComboPhase = 0;      // 1段の中でどのフェーズか（振りかぶりなど）
		bool comboNext = false;        // 次のコンボに進むかどうかのフラグ
	};
	struct _WorkAttack {
		uint32_t attackParameter_ = 0; // コンボ進行を管理するパラメーター
		int32_t comboIndex = 0;        // 現在のコンボ段階（0, 1, 2...）
		ComboPhase inComboPhase = ComboPhase::kAnticipation;      // 1段の中でどのフェーズか（振りかぶりなど）
		bool comboNext = false;        // 次のコンボに進むかどうかのフラグ
	};

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
	/// ジャンプ初期化
	/// </summary>
	void BehaviorJumpInit();

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate();

	/// <summary>
	/// ジャンプ更新
	/// </summary>
	void BehaviorJumpUpdate();

	/// <summary>
	/// 自機の浮遊
	/// </summary>
	void UpdateFloating();

public: // アクセッサ1

	Vector3 GetPosition() { return transform_.translation_; }
	Vector3 GetRotation() { return transform_.rotation_; }
	Vector3 GetWeaponPosition() const{return weapon_->GetTranslation();}
	Weapon* GetWeapon() { return weapon_.get(); }

private: // メンバ変数
	// ポインタ
	std::unique_ptr<Weapon> weapon_;

	// 各オブジェクト
	Object3d* baseObject_ = nullptr;
	Object3d* headObject_ = nullptr;
	Object3d* bodyObject_ = nullptr;
	Object3d* L_arm_Object_ = nullptr;
	Object3d* R_arm_Object_ = nullptr;

	

	Model* models_;
	Input* input_ = nullptr;
	Vector3 moveSpeed_;

	// メインのtransform
	WorldTransform transform_;
	WorldTransform body_transform_;
	WorldTransform head_transform_;
	WorldTransform L_arm_transform_;
	WorldTransform R_arm_transform_;

public:

	// プレイヤーの浮遊用パラメータ
	float playerFloatingParameter_ = 0.0f; // プレイヤーの浮遊パラメータ
	const float playerFloatingAmplitude_ = 0.1f; // プレイヤーの浮遊振幅
	const float playerFloatingPeriod_ = 60; // プレイヤーの浮遊周期
	const float playerFloatingStep_ = 2.0f * static_cast<float>(std::numbers::pi) / static_cast<float>(playerFloatingPeriod_);

	// 武器の浮遊用パラメータ
	float weaponFloatingParameter_ = 0.0f; // 武器の浮遊パラメータ
	const float weaponFloatingAmplitude_ = 0.15f; // 武器の浮遊振幅
	const float weaponFloatingPeriod_ = 80; // 武器の浮遊周期
	const float weaponFloatingStep_ = 2.0f * static_cast<float>(std::numbers::pi) / static_cast<float>(weaponFloatingPeriod_);

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviortRquest_ = std::nullopt;

	// 速度
	Vector3 velocity_ = {};

	// 攻撃速度
	float attackSpeed_ = 0.25f;
	// コンボの最大数
	static const int ComboNum = 3;

	static const std::array<ConstAttack, ComboNum> kConstAttacks_;

	WorkAttack workAttack_;

	_WorkAttack _workAttack_;
};					 

