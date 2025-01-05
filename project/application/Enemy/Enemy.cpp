#include "Enemy.h"
#include "../player/Player.h"
#include "imgui.h"
#include "../global/CollisionTypeIdDef.h"
#include "EnemyManager.h"
#include "operatorOverload.h"
#include "stdio.h"
// 次のシリアルナンバー
uint32_t Enemy::nextSerialNumber_ = 0;
Enemy::Enemy()
{
	// シリアルナンバーを振る
	serialNumber_ = nextSerialNumber_;
	// 番号の追加
	++nextSerialNumber_;
}

Enemy::~Enemy()
{
}

void Enemy::Initialize(const Vector3& pos)
{
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModel("float_head.obj");
	//object_->SetTranslate(pos);
	//transform_ = { object_->GetScale(),object_->GetRotation() ,object_->GetTranslate() };
	transform_.Initialize();
	transform_.translation_ = pos;
	//transform_.translation_.y = transform_.translation_.y - 2.0f;
	// 種別のIDの設定
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	


}

void Enemy::Update()
{
	BehaviorInitialize();

	BehaviorUpdate();

	UpdateTransform();
#ifdef _DEBUG

	ImGui::Begin("enemy");
	ImGui::InputFloat3("pos", &transform_.translation_.x);
	ImGui::Checkbox("onLight", &onLight_);
	ImGui::End();

#endif // _DEBUG

}

void Enemy::UpdateTransform()
{
	transform_.TransferMatrix();
}

void Enemy::Draw()
{
	if(behavior_ != EnemyBehavior::kLight)
	{
		object_->Draw(transform_);
	}
}


void Enemy::BehaviorInitialize()
{
	if (behaviortRquest_) {
		// 振る舞いを変更する
		behavior_ = behaviortRquest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case EnemyBehavior::kRoot:
		default:
			BehaviorRootInit();
			break;
		case EnemyBehavior::kKnockBack:
			BehaviorKnockBackInit();
			break;
		case EnemyBehavior::kCorpse:
			BehaviorCorpseInit();
			break;
		case EnemyBehavior::kLight:
			BehaviorLightInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviortRquest_ = std::nullopt;
	}
}

void Enemy::OnCollision([[maybe_unused]] Collider* other)
{
	// 当たったのが武器の場合のみ
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon) &&
		player_->behavior_ == Behavior::kAttack)
	{
		switch (behavior_) {
		case EnemyBehavior::kRoot:
		case EnemyBehavior::kCorpse:
			onCollision_ = true;
			break;
		default:
			onCollision_ = false;
			break;
		}
	}
}

Vector3 Enemy::GetCenterPosition() const
{
	// 見た目上の中心点のオフセット（モデル座標系）
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	// ワールド座標系に変換
	Vector3 worldPos = TransformM(offset, transform_.matWorld_);
	return worldPos;
}

void Enemy::Move()
{
	if (!onLight_) {
		speed_ = kNomalSpeed_;
	}
	else {
		speed_ = kOnLightSpeed_;
	}
	Vector3 playerPos = player_->GetPosition();
	playerPos = playerPos - Vector3{ 0,1.0f,0 };
	Vector3 pos = { playerPos - transform_.translation_ };
	float weponRadius = 4.0f;
	if (Length(pos) > weponRadius + radius_)// プレイヤーの半径 + エネミー半径 - 0.1fくらい？
	{
		pos = Normalize(pos);
		transform_.translation_ += pos * speed_;
	}
	Turning();
}

void Enemy::Turning() {
	Vector3 lockOnPos = player_->GetPosition();
	Vector3 sub = lockOnPos - transform_.translation_;
	transform_.rotation_.y = std::atan2(sub.x, sub.z);
}

void Enemy::BehaviorUpdate()
{
	switch (behavior_) {
	case EnemyBehavior::kRoot:
	default:
		// 通常行動更新
		BehaviorRootUpdate();
		break;
	case EnemyBehavior::kKnockBack:
		BehaviorKnockBackUpdate();
		break;
	case EnemyBehavior::kCorpse:
		BehaviorCorpseUpdate();
		break;
	case EnemyBehavior::kLight:
		BehaviorLightUpdate();
		break;
	}
}

void Enemy::BehaviorRootInit()
{
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	onLight_ = false;
	onCollision_ = false;
	Turning();
}

void Enemy::BehaviorKnockBackInit()
{
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	paramater_ = step_;
	knockBackVec_ = Normalize(transform_.translation_ - player_->GetPosition());
	startY_ = transform_.translation_.y + 2;
	transform_.translation_.y = startY_;
	onCollision_ = false;
}

void Enemy::BehaviorCorpseInit()
{
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	corpseTimer_ = kMaxCorpseTimer_;
	onCollision_ = false;
}

void Enemy::BehaviorLightInit()
{
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	lightTimer_ = kMaxLightTimer_;
	onCollision_ = false;
}

void Enemy::BehaviorRootUpdate()
{
	if (onCollision_)
	{
		behaviortRquest_ = EnemyBehavior::kKnockBack;
	}
	Move();
}

void Enemy::BehaviorKnockBackUpdate()
{
	transform_.translation_.y = startY_ + (-2.0f * easeOutBounce(paramater_ / knockBackTime_));
	transform_.translation_.x += knockBackVec_.x * kKnockBackSpeed_;
	transform_.translation_.z += knockBackVec_.z * kKnockBackSpeed_;
	if (paramater_ < knockBackTime_)
	{
		paramater_ += step_;
	}
	else {
		if(!onLight_)
		{
			behaviortRquest_ = EnemyBehavior::kCorpse;
		}
		else {
			isDelete_ = true;
		}
	}
}

void Enemy::BehaviorCorpseUpdate()
{
	if (corpseTimer_ > 0) {
		corpseTimer_--;
	}
	else {
		isDelete_ = true;
	}
	if (onCollision_)
	{
		behaviortRquest_ = EnemyBehavior::kLight;
	}
}

void Enemy::BehaviorLightUpdate()
{
	if (lightTimer_ > 0) {
		lightTimer_--;
	}
	else {
		isDelete_ = true;
	}
}

float Enemy::easeOutBounce(float x)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1) {
		return n1 * x * x;
	}
	else if (x < 2 / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}
