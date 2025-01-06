#include "IScene.h"

int IScene::sceneNo_ = TITLE;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sceneNo_; }
