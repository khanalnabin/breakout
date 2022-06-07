#ifndef GAME_H_
#define GAME_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "game_level.h"

// Represents the current state of the game
enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

// Main Game Class
class Game {
  public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;

	std::vector<GameLevel> Levels;
	unsigned int Level;

	Game(unsigned int width, unsigned int height);
	~Game();

	// initialize game state (load all shaders / textures / levels)
	void Init();

	// game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void DoCollisions();

	void ResetPlayer();
	void ResetLevel();
};
#endif // GAME_H_
