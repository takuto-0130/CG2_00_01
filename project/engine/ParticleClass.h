#include <DirectXBasis.h>
#include <SrvManager.h>
#include <random>
#include <struct.h>
#include <Camera.h>

const uint32_t kNumMaxInstance = 100; // インスタンス数
const float kDeltaTime = 1.0f / 60.0f;



#pragma once
class ParticleClass
{
public:
	// 初期化
	void Initialize(DirectXBasis* dxBasis, SrvManager* srvManager);
	// 更新
	void Update();
	// 描画
	void Draw();

	// カメラをセットする
	void SetCamera(Camera* camera) { camera_ = camera; }

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

	Particle MakeNewParticle(std::mt19937& random, const Vector3& translate);
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& random);
	bool IsCollision(const AABB& a, const Vector3& point);

private:

	ModelData modelData;

	Camera* camera_;

	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

	std::list<Particle> particles;
	std::random_device seedGene;
	//std::mt19937 random(seedGene());


	Emitter emitter{};

	AccelerationField accel;
	bool isAccel = false;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	bool useBillboard = false;
};

