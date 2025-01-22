#pragma once
#include "struct.h"
#include <random>
class CameraShake
{
public:
	static CameraShake* GetInstance()
	{
		static CameraShake instance;
		return &instance;
	}

private:
	CameraShake() = default;
	~CameraShake() = default;
	CameraShake(CameraShake&) = default;
	CameraShake& operator=(CameraShake&) = default;
public:
	void Update();

	void SetShake(const Vector3& shakeRange, const float& shakeTime);

	Vector3 GetShake() { return shakeVec_; }

	bool IsShake() { return isShake_; }

private:
	Vector3 shakeVec_ = {};
	Vector3 shakeRange_ = {};
	float shakeTime_ = 0;
	float shakeLocalTimer_ = 0;
	const float deltaTime_ = 1.0f / 60.0f;
	bool isShake_ = false;
	std::random_device seedGene_;
};

