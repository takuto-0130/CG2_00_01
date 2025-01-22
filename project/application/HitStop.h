#pragma once
class HitStop
{
public:
	static HitStop* GetInstance()
	{
		static HitStop instance;
		return &instance;
	}

private:
	HitStop() = default;
	~HitStop() = default;
	HitStop(HitStop&) = default;
	HitStop& operator=(HitStop&) = default;
public:
	void Update();

	void SetHitStop(const float& duration);

	bool IsHitStop() { return isHitStop_; }

private:
	bool isHitStop_ = false;
	const float deltaTime_ = 1.0f / 60.0f;
	float hitStopDuration_ = 0;
	float localTimer_ = 0;
};

