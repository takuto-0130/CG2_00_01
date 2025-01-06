#include "IScene.h"

int IScene::sceneNo_ = STAGE;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sceneNo_; }
