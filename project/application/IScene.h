#pragma once
#include "Input.h"
#include "Camera.h"
#include "Fade.h"

enum Scene {TITLE, STAGE, CLEAR};
class IScene {
protected:
	static int sceneNo_;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;

public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	void SetCamera(Camera* camera) { camera_ = camera; };

	virtual ~IScene();

	int GetSceneNo();
};
