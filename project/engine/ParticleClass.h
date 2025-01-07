#include <DirectXBasis.h>
#include <SrvManager.h>
#include <random>
#include <struct.h>
#include <Camera.h>

#include "WorldTransform.h"
#include "Object3d.h"

const uint32_t kNumMaxInstance = 500; // インスタンス数
const float kDeltaTime = 1.0f / 60.0f;



#pragma once
class ParticleClass
{
private:

	struct Particle {
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct Emitter {
		Transform transform;
		uint32_t count;
		float frequency;
		float frequencyTime;
	};

	struct AABB {
		Vector3 min;
		Vector3 max;
	};

	struct AccelerationField {
		Vector3 acceleration;
		AABB area;
	};
private:

	ParticleClass() = default;
	~ParticleClass() = default;
	ParticleClass(ParticleClass&) = default;
	ParticleClass& operator=(ParticleClass&) = default;


public:
	// シングルトンインスタンスの取得
	static ParticleClass* GetInstance()
	{
		static ParticleClass instance; // 静的ローカル変数
		return &instance;
	}
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// カメラをセットする
	void SetCamera(Camera* camera) { camera_ = camera; }

	// エミッターをの設定を変更
	void SetEmitter(Emitter emitter) { emitter_ = emitter; }

	void CollisionEmit(Vector3 pos);

private:

	Particle MakeNewParticle(std::mt19937& random, const Vector3& translate);
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& random);
	bool IsCollision(const AABB& a, const Vector3& point);

private:
	Matrix4x4 backToFrontMatrix = MakeIdentity4x4();
	Matrix4x4 billboardMatrix = MakeIdentity4x4();
	std::unique_ptr<Object3d> obj_;
	WorldTransform trans_[300];


	std::list<Particle> particles;
	std::random_device seedGene;


	Emitter emitter_{};

	Camera* camera_ = nullptr;

	Transform transform = {};

	uint32_t numInstance = 0;

	bool isEmit_ = false;
};

