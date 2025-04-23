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

	// カメラ座標
	struct CameraForGPUP {
		Vector3 worldPosition;
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
	void Initialize(DirectXBasis* dxBasis, SrvManager* srvManager, Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();

	void CreateRootSignature();

	D3D12_INPUT_LAYOUT_DESC CreateInputElementDesc();

	void CreateBlendState();

	void CreateRasterizerState();

	void LoadShader();

	void CreatePipelineState();

	void CreateParticleResource();

	void CreateMaterialResource();

	void CreateCameraResource();

	// カメラをセットする
	void SetCamera(Camera* camera) { camera_ = camera; }

	// エミッターをの設定を変更
	void SetEmitter(Emitter emitter) { emitter_ = emitter; }

private:

	Particle MakeNewParticle(std::mt19937& random, const Vector3& translate);
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& random);
	bool IsCollision(const AABB& a, const Vector3& point);

	constexpr UINT AlignTo256(UINT size) {
		return (size + 255) & ~255;
	}

private:

	ModelData modelData;

	uint32_t srvIndex = 0;


	std::list<Particle> particles;
	std::random_device seedGene;


	Emitter emitter_{};

	AccelerationField accel = {};
	bool isAccel = false;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = {};
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = {};

	bool useBillboard = false;


	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	// PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	// BlendState
	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	DirectXBasis* dxBasis_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	Camera* camera_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	ParticleForGPU* instancingData_ = nullptr;
	Material* materialData_ = nullptr;
	CameraForGPUP* cameraData_ = nullptr;
	VertexData* vertexData_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	Transform transform = {};

	uint32_t numInstance = 0;
};

