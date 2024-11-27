#include "GameCore.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<TYFrameWork> game = std::make_unique<GameCore>();

	game->run();
	
	return 0;
}