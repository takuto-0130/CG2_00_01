#include "EnemyManager.h"
#include "../player/Player.h"
#include "imgui.h"
#include "mathFunc.h"
#include "operatorOverload.h"


EnemyManager::~EnemyManager()
{
	enemys_.remove_if([](const std::shared_ptr<Enemy>& enemy)
		{
			return true;
		});
}

void EnemyManager::Initialize()
{
	enemys_.remove_if([](const std::shared_ptr<Enemy>& enemy) 
		{
		return true;
		});

	Timer_ = 0;
	eliminateCount_ = 0;
}

void EnemyManager::Update()
{
	RandomPop();


	for (std::shared_ptr<Enemy> enemy : enemys_) 
	{
		if (enemy->IsDelete()) 
		{
			eliminateCount_++;
		}
	}

	enemys_.remove_if([](const std::shared_ptr<Enemy>& enemy)
		{
			return enemy->IsDelete();
		});

	for (std::shared_ptr<Enemy> enemy : enemys_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (std::shared_ptr<Enemy> enemy : enemys_) 
	{
		enemy->Draw();
	}
}

void EnemyManager::RandomPop()
{
	Timer_++;
	std::uniform_int_distribution<> distrib(1, kMaxAngle);
	std::mt19937 gen(rd_());
	int angle = distrib(gen);
	if(Timer_%interval_ == 0 && Timer_ < maxPopTime_)
	{
		Vector3 pos = TransformVector3(kPopPos, MakeRotateYMatrix(popAngle_ * static_cast<float>(angle))) + player_->GetPosition();
		PopEnemy(pos);
	}
}

void EnemyManager::PopEnemy(Vector3 position) 
{

	std::shared_ptr<Enemy> enemy;
	enemy.reset(new Enemy);
	enemy->Initialize(position);
	enemy->SetPlayer(player_);

	enemys_.push_back(enemy);
}

Vector3 EnemyManager::TransformVector3(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

void EnemyManager::AddCollider()
{
	for (std::shared_ptr<Enemy> enemy : enemys_) 
	{
		collisionManager_->AddCollider(enemy.get());
	}
}

std::vector<Collider*> EnemyManager::GetColliders()
{
	std::vector<Collider*> colliders;
	for (auto& enemy : enemys_) 
	{
		colliders.push_back(enemy.get());
	}
	return colliders;
}
