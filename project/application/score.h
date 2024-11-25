#pragma once
#include "Sprite.h"
#include <memory>
class score
{
public:
	void Initialze();

	void Update();

	void Draw();

	struct ScoreDisp {
		std::array<int32_t, 4> num;
	};

	void SetScore(const int32_t& score) { score_ = score; }

private:
	void ScoreDisplay();

private:
	std::unique_ptr<Sprite> one_;
	std::unique_ptr<Sprite> two_;
	std::unique_ptr<Sprite> three_;
	std::unique_ptr<Sprite> four_;
	std::unique_ptr<Sprite> back_;
	int32_t score_;
	ScoreDisp scoreDisp_;
};

