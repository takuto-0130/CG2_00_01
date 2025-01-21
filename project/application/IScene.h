#pragma once
#include "Input.h"
#include "Camera.h"

class SceneManager;

class IScene {
protected:
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	void SetCamera(Camera* camera) { camera_ = camera; };

	virtual ~IScene() = default;
};
