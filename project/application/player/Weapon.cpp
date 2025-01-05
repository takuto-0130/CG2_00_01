#include "Weapon.h"
#include "ModelManager.h"
#include "mathFunc.h"
#include "Matrix4x4Func.h"
#include "../Enemy/Enemy.h"

Weapon::~Weapon(){}

void Weapon::Initialize()
{
	object_ = new Object3d();
	object_->Initialize();
	
	ModelManager::GetInstance()->LoadModel("Resources","hammer.obj");
	object_->SetModel("hammer.obj");
	transform_.Initialize();
	transform_.scale_ = { 0.7f,0.7f,0.7f };
	Collider::Initialize();
	// 種別のIDの設定
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	isHit_ = false;
}

void Weapon::Update()
{
	float offset = 0.3f;
	transform_.translation_.x = offset;
	transform_.TransferMatrix();
}

void Weapon::Draw()
{
	object_->Draw(transform_);
}

Vector3 Weapon::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.3f, 3.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = TransformM(offset, transform_.matWorld_);
	return worldPos;
}

void Weapon::OnCollision(Collider* other)
{
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		uint32_t serialNumber = enemy->GetSerialNumber();
	
		// 接触履歴があれば何もせずに抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}
		// 接触履歴に登録
		contactRecord_.record(serialNumber);
		isHit_ = true;
		
	}

}

void Weapon::RecordClear()
{
	// 接触履歴を抹消
	contactRecord_.Clear();
}
