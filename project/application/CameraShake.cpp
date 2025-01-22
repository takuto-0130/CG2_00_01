#include "CameraShake.h"

void CameraShake::Update()
{
	if (isShake_) 
	{
		if(shakeTime_ > shakeLocalTimer_)
		{
			float t = 1.0f - (shakeLocalTimer_ / shakeTime_);

			std::mt19937 random(seedGene_());

			std::uniform_real_distribution<float> shakeX(-shakeRange_.x, shakeRange_.x);
			shakeVec_.x = shakeX(random) * t;

			std::uniform_real_distribution<float> shakeY(-shakeRange_.y, shakeRange_.y);
			shakeVec_.y = shakeY(random) * t;

			std::uniform_real_distribution<float> shakeZ(-shakeRange_.z, shakeRange_.z);
			shakeVec_.z = shakeZ(random) * t;


			shakeLocalTimer_ += deltaTime_;
		}
		else 
		{
			isShake_ = false;
			shakeVec_ = { 0,0,0 };
			shakeLocalTimer_ = 0;
		}
	}
}

void CameraShake::SetShake(const Vector3& shakeRange, const float& shakeTime)
{
	isShake_ = true;
	shakeTime_ = shakeTime;
	shakeRange_ = shakeRange;
	shakeLocalTimer_ = 0;
}
