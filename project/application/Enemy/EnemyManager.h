#pragma once
#include "Enemy.h"
#include <numbers>
#include <random>
#include "../global/CollisionManager.h"

class EnemyManager
{
public:
	~EnemyManager();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	/// <summary>
	/// プレイヤーのセッター
	/// </summary>
	void SetPlayer(Player* player) { player_ = player; }

	void AAA();
	std::vector<Collider*>GetColliders();

private:

	// 出現(ランダム)
	void RandomPop();

	// 出現処理
	void PopEnemy(Vector3 position);


	Vector3 TransformVector3(const Vector3& vector, const Matrix4x4& matrix);

	

private:
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;
	std::list<std::shared_ptr<Enemy>> enemys_;

	int32_t Timer_;
	int32_t interval_ = 20;


	Player* player_;

	float popAngle_ = std::numbers::pi_v<float> / 180.0f;

	const int kMaxAngle = 360;
	Vector3 kPopPos = { 0 ,0 ,50.0f };
	std::random_device rd_;

};

