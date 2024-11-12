#pragma once
#include "Sprite.h"
#include <memory>
class score
{
public:
	void Initialze();

	void Update();

	void Draw();
private:
	std::unique_ptr<Sprite> one_;
	std::unique_ptr<Sprite> two_;
	std::unique_ptr<Sprite> three_;
	std::unique_ptr<Sprite> four_;
	std::unique_ptr<Sprite> back_;
};

