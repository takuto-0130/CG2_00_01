#include "HitStop.h"

void HitStop::Update()
{
	if (isHitStop_) 
	{
		if (hitStopDuration_ > localTimer_) 
		{
			localTimer_ += deltaTime_;
		}
		else 
		{
			isHitStop_ = false;
		}
	}
}

void HitStop::SetHitStop(const float& duration)
{
	isHitStop_ = true;
	hitStopDuration_ = duration;
	localTimer_ = 0;
}
