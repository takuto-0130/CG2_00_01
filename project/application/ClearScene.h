#pragma once
#include "IScene.h"
class ClearScene : public IScene {
public:
	void Init() override;
	void Update() override;
	void Draw() override;
};
