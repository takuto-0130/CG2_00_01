#include <DirectXBasis.h>
#include <SrvManager.h>
#include <random>
#include <struct.h>
#include <Camera.h>

const uint32_t kNumMaxInstance = 100; // インスタンス数


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

	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

	std::list<Particle> particles;
	std::random_device seedGene;

	Emitter emitter{};

	AccelerationField accel;
};

