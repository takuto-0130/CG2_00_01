#pragma once
#include "IScene.h"

enum class TitlePhase {
	kFadeIn,
	kTitle,
	kFadeOut
};
class TitleScene : public IScene {
public:
	TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	void ChangePhase();

private:
	std::unique_ptr<Fade> fade_;
	TitlePhase phase_ = TitlePhase::kFadeIn;
	std::unique_ptr<Sprite> titleSprite_;
};
