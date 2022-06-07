#ifndef GAME_LEVEL_H_
#define GAME_LEVEL_H_

#include <vector>

#include "game_object.h"
#include "resource_manager.h"

class GameLevel {
  public:
	std::vector<GameObject> Bricks;
	GameLevel();

	void Load(const char *file, unsigned int levelWidth,
	          unsigned int levelHeight);

	void Draw(SpriteRenderer &renderer);

	bool IsCompleted();

  private:
	void init(std::vector<std::vector<unsigned int>> tileData,
	          unsigned int levelWidth, unsigned int levelHeight);
};
#endif // GAME_LEVEL_H_
