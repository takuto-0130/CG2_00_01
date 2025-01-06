#include "Player.h"
#include "Xinput.h"
#ifdef _DEBUG
#include <imgui.h>
#endif 
#include "GlobalVariables.h"
#include <numbers>

const std::array<Player::ConstAttack, Player::ComboNum> Player::kConstAttacks_ = {
	{
		{0, 0, 10, 30, 0.0f, 0.0f, 0.15f},  // 1段目
		{15, 10, 15, 50, 0.8f, 0.0f, 0.0f}, // 2段目
		{15, 10, 15, 30, 0.2f, 0.0f, 0.0f} // 3段目
	}
};
Player::~Player()
{
	delete baseObject_;
	delete headObject_;
	delete bodyObject_;
	delete L_arm_Object_;
	delete R_arm_Object_;
}
void Player::Initialize()
{
	// obj
	baseObject_ = new Object3d();
	headObject_ = new Object3d();
	bodyObject_ = new Object3d();
	L_arm_Object_ = new Object3d();
	R_arm_Object_ = new Object3d();

	baseObject_->Initialize();
	headObject_->Initialize();
	bodyObject_->Initialize();
	L_arm_Object_->Initialize();
	R_arm_Object_->Initialize();

	ModelManager::GetInstance()->LoadModel("Resources", "float_head.obj");
	headObject_->SetModel("float_head.obj");
	ModelManager::GetInstance()->LoadModel("Resources", "float_body.obj");
	bodyObject_->SetModel("float_body.obj");
	ModelManager::GetInstance()->LoadModel("Resources", "float_L_arm.obj");
	L_arm_Object_->SetModel("float_L_arm.obj");
	ModelManager::GetInstance()->LoadModel("Resources", "float_R_arm.obj");
	R_arm_Object_->SetModel("float_R_arm.obj");


	// ワールドトランスフォームの初期化
	transform_.Initialize();
	body_transform_.Initialize();
	head_transform_.Initialize();
	L_arm_transform_.Initialize();
	R_arm_transform_.Initialize();

	// 親子付け
	body_transform_.parent_ = &transform_;
	head_transform_.parent_ = &body_transform_;
	L_arm_transform_.parent_ = &body_transform_;
	R_arm_transform_.parent_ = &body_transform_;

	// 腕の位置をセット
	L_arm_transform_.translation_ = { -0.5f ,1.0f,0.0f };
	R_arm_transform_.translation_ = { 0.5f ,1.0f,0.0f };

	// 武器
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize();
	weapon_->SetTranslation(Vector3{ -0.5f,-0.5f,0.0f });
	weapon_->SetRotation(Vector3{ 1.5f,0.0f,0.0f });
	
	weapon_->SetParent(R_arm_transform_);

	// その他機能の初期化
	input_ = Input::GetInstance();
	moveSpeed_ = { 0.3f, 0.3f ,0.3f };

	// 通常モーションで初期化
	Behavior behavior_ = Behavior::kRoot;

	// 浮遊パラメーター初期化
	playerFloatingParameter_ = 0.0f;
	weaponFloatingParameter_ = 0.0f;

	GlobalVariables* globalvariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	//globalvariables->AddItem(groupName, "Translation", transform_.translation_);
	//globalvariables->AddItem(groupName, "Body Translation", worldTransformBody_.translation_);
	//globalvariables->AddItem(groupName, "Head Translation", worldTransformHead_.translation_);
	//globalvariables->AddItem(groupName, "ArmL Translation", worldTransformL_arm_.translation_);
	//globalvariables->AddItem(groupName, "ArmR Translation", worldTransformR_arm_.translation_);
	//globalvariables->AddItem(groupName, "floatingCycle", floatingParameter_);
	//globalvariables->AddItem(groupName, "floatingAmplitude", amplitude_);
	//globalvariables->AddItem(groupName, "idleArmAngleMax", armAmplitude_);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));
}

void Player::Update()
{
	// 各行動の初期化
	BehaviorInitialize();

	// 各行動の更新
	BehaviorUpdate();

	// 末端処理）
	LastUpdate();
}

void Player::Draw()
{
	// 各オブジェクトの描画
	headObject_->Draw(head_transform_);
	bodyObject_->Draw(body_transform_);
	L_arm_Object_->Draw(L_arm_transform_);
	R_arm_Object_->Draw(R_arm_transform_);
	if (behavior_ == Behavior::kAttack) {
		weapon_->Draw();
	}
}

void Player::OnCollision(Collider* other)
{
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {

	//	behaviortRquest_ = Behavior::kJump;
	}
}

Vector3 Player::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.5f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = TransformM(offset, transform_.matWorld_);

	return worldPos;
}

void Player::LastUpdate()
{
	MoveLimit();
	// 各オブジェクトの更新
	transform_.TransferMatrix();
	body_transform_.TransferMatrix();
	head_transform_.TransferMatrix();
	L_arm_transform_.TransferMatrix();
	R_arm_transform_.TransferMatrix();
	weapon_->Update();
}

void Player::Move()
{
	Vector3 direction = {}; // プレイヤーの移動方向を計算するベクトル

	// キーボードの入力で移動と方向の決定
	if (input_->PushKey(DIK_W)) {
		transform_.translation_.z += moveSpeed_.z;
		direction.z += 1.0f;
	}
	if (input_->PushKey(DIK_A)) {
		transform_.translation_.x -= moveSpeed_.x;
		direction.x -= 1.0f;
	}
	if (input_->PushKey(DIK_S)) {
		transform_.translation_.z -= moveSpeed_.z;
		direction.z -= 1.0f;
	}
	if (input_->PushKey(DIK_D)) {
		transform_.translation_.x += moveSpeed_.x;
		direction.x += 1.0f;
	}

	float length = Length(direction);
	// 移動している場合に方向ベクトルから回転を計算
	if (length > 0) {
		// 向きの回転角度を計算（例: Y軸回転）
		float angle = atan2(direction.x, direction.z); // Z軸が前と仮定

		// プレイヤーの回転を設定
		transform_.rotation_.y = angle;
	}
}

/// <summary>
/// モーション全体の初期化
/// </summary>
void Player::BehaviorInitialize()
{
	if (behaviortRquest_) {
		// 振る舞いを変更する
		behavior_ = behaviortRquest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInit();
			break;
		case Behavior::kAttack:
			BehaviorAttackInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviortRquest_ = std::nullopt;
	}
}

/// <summary>
/// モーション全体の更新
/// </summary>
void Player::BehaviorUpdate() {
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動更新
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		// 攻撃行動更新
		BehaviorAttackUpdate();
		break;
	}
}

void Player::BehaviorRootInit()
{
	playerFloatingParameter_ = 0.0f;
	weaponFloatingParameter_ = 0.0f;
	weapon_->RecordClear();
	R_arm_transform_.rotation_.x = 0.0f;
	L_arm_transform_.rotation_.x = 0.0f;
}

void Player::BehaviorAttackInit()
{
	weaponFloatingParameter_ = 0.0f;
	R_arm_transform_.rotation_.x = 0.0f;
	L_arm_transform_.rotation_.x = 0.0f;
	weapon_->RecordClear();
	attackSpeed_ = 0.25f;
}

void Player::BehaviorRootUpdate()
{
	// 移動
	Move();

	// 浮遊
	UpdateFloating();

	// 攻撃ボタンが押されたらコンボを開始
	if (input_->TriggerKey(DIK_SPACE)) {
		// コンボの最初の段階に設定
		workAttack_.comboIndex = 0;
		workAttack_.inComboPhase = 0;
		workAttack_.attackParameter_ = 0;
		workAttack_.comboNext = false;
		behaviortRquest_ = Behavior::kAttack;
	}
}

void Player::BehaviorAttackUpdate()
{
	// 現在のコンボ情報を取得
	const ConstAttack* attack = kConstAttacks_.data();
	// 攻撃中に次の攻撃入力を受け付ける
	if (input_->TriggerKey(DIK_SPACE)) {
		workAttack_.comboNext = true;  // 次のコンボへ進む準備
	}
	float swingRotare = std::numbers::pi_v<float> / 2.0f;
	if (workAttack_.comboIndex == 0) {
		// コンボの各フェーズを進行
		if (workAttack_.inComboPhase == 0) {
			// 1段目: 振りかぶりの動作
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].anticipationTime) {
				workAttack_.inComboPhase++;
				workAttack_.attackParameter_ = 0;
			}
			// 腕を振り上げるモーションやプレイヤーの回転
			R_arm_transform_.rotation_.x = -swingRotare;
		}
		else if (workAttack_.inComboPhase == 1) {
			// 攻撃の溜めるモーションやエフェクトの発生
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].chargeTime) {
				workAttack_.inComboPhase++;
				workAttack_.attackParameter_ = 0;
			}
		}
		else if (workAttack_.inComboPhase == 2) {
			// 攻撃モーションやエフェクトの発生
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].swingTime) {
				workAttack_.inComboPhase++;
				workAttack_.attackParameter_ = 0;
			}
			// 例: プレイヤーが前に出て武器を振る
			R_arm_transform_.rotation_.x += swingRotare / float(attack[workAttack_.comboIndex].swingTime);
		}
		else if (workAttack_.inComboPhase == 3) {
			// 攻撃後の硬直等
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].recoveryTime) {
				// コンボ終了時に戻る
				if (workAttack_.comboIndex >= ComboNum || workAttack_.comboNext == false) {
					workAttack_.comboIndex = 0;
					workAttack_.inComboPhase = 0;
					behavior_ = Behavior::kRoot;
					R_arm_transform_.rotation_.x = 0.0f;
					L_arm_transform_.rotation_.x = 0.0f;
				}
				else {
					workAttack_.comboNext = false;
					workAttack_.comboIndex++;
					workAttack_.attackParameter_ = 0;
					workAttack_.inComboPhase = 0;
				}

			}
			// 硬直中の静止モーションなど
		}
	}

	// 2段目以降も同様
	if (workAttack_.comboIndex == 1) {
		// 2段目のコンボ処理
		if (workAttack_.inComboPhase == 0) {
			Vector3 moveVec = { 0,0,attack[workAttack_.comboIndex].anticipationSpeed };
			// 2段目: 振りかぶりの動作
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].anticipationTime) {
				workAttack_.inComboPhase++;
				workAttack_.attackParameter_ = 0;
			}
			// 例: 2段目の振りかぶりモーション
			R_arm_transform_.rotation_.x = -swingRotare;
			R_arm_transform_.rotation_.z = swingRotare;
			body_transform_.rotation_.y = -0.45f;

			Matrix4x4 rotaY = MakeRotateYMatrix(transform_.rotation_.y);
			transform_.translation_ += TransformM(moveVec, rotaY);
		}
		else if (workAttack_.inComboPhase == 1) {
			// 2段目: 攻撃振りの動作
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].swingTime) {
				workAttack_.inComboPhase++;
				workAttack_.attackParameter_ = 0;
			}
			// 例: 2段目の攻撃モーション
			body_transform_.rotation_.y += 0.06f;
			R_arm_transform_.rotation_.x += 0.2f;
		}
		else if (workAttack_.inComboPhase == 2) {
			// 2段目: 硬直状態
			if (++workAttack_.attackParameter_ >= attack[workAttack_.comboIndex].recoveryTime) {
				workAttack_.comboNext = false;
				/*workAttack_.comboIndex++;
				workAttack_.attackParameter_ = 0;*/
				workAttack_.comboIndex = 0;
				behavior_ = Behavior::kRoot;
				R_arm_transform_.rotation_.x = 0;
				R_arm_transform_.rotation_.z = 0;
				body_transform_.rotation_.y = 0;
				if (workAttack_.comboIndex >= ComboNum) {
					workAttack_.comboIndex = 0;
					behavior_ = Behavior::kRoot;
				}
			}
		}
	}
	// 3段目

}

void Player::UpdateFloating()
{
	// プレイヤーの浮遊
	playerFloatingParameter_ += playerFloatingStep_;
	playerFloatingParameter_ = static_cast<float>(fmod(playerFloatingParameter_, 2.0f * std::numbers::pi));

	// 浮遊をプレイヤーのY座標に反映
	body_transform_.translation_.y = sin(playerFloatingParameter_) * playerFloatingAmplitude_;

	// 武器の浮遊
	weaponFloatingParameter_ += weaponFloatingStep_;
	weaponFloatingParameter_ = static_cast<float>(fmod(weaponFloatingParameter_, 2.0f * std::numbers::pi));

	// 浮遊を武器のY座標に反映
	Vector3 weaponTranslation = weapon_->GetTranslation();
	weapon_->SetTranslation(Vector3{ weaponTranslation.x, sin(weaponFloatingParameter_) * weaponFloatingAmplitude_, weaponTranslation.z });
}

void Player::MoveLimit()
{
	transform_.translation_.x = std::clamp(transform_.translation_.x, -moveLimit_, moveLimit_);
	transform_.translation_.z = std::clamp(transform_.translation_.z, -moveLimit_, moveLimit_);
}
