#pragma once
#include "IScene.h"

enum class ClearPhase {
	kFadeIn,
	kClear,
	kFadeOut
};
class ClearScene : public IScene {
public:
	void Init() override;
	void Update() override;
	void Draw() override;

private:
	void ChangePhase();

private:
	std::unique_ptr<Fade> fade_;
	ClearPhase phase_ = ClearPhase::kFadeIn;
	std::unique_ptr<Sprite> clearSprite_;
};
