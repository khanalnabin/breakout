#include "../include/game.h"
#include "../include/resource_manager.h"
#include "../include/sprite_renderer.h"
#include "../include/ball_object.h"

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

SpriteRenderer *Renderer;
GameObject *Player;
BallObject *Ball;

enum Direction { UP, RIGHT, DOWN, LEFT };
typedef std::tuple<bool, Direction, glm::vec2> Collision;
Direction VectorDirection(glm::vec2 target);

Collision CheckCollision(BallObject &one, GameObject &two);

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {}

Game::~Game() { delete Renderer; }

void Game::Init() {
	// load shader
	ResourceManager::LoadShader("res/shaders/sprite/vertex.glsl",
	                            "res/shaders/sprite/fragment.glsl", nullptr,
	                            "sprite");
	ResourceManager::LoadTexture("res/textures/background.jpg", false,
	                             "background");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("res/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("res/textures/block.png", false, "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", false,
	                             "block_solid");
	GameLevel one, two, three, four;
	one.Load("res/levels/one.lvl", this->Width, this->Height / 2);
	two.Load("res/levels/two.lvl", this->Width, this->Height / 2);
	three.Load("res/levels/three.lvl", this->Width, this->Height / 2);
	four.Load("res/levels/four.lvl", this->Width, this->Height / 2);

	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);

	this->Level = 0;

	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
	                                this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE,
	                        ResourceManager::GetTexture("paddle"));

	// configure shaders
	glm::mat4 projection =
	    glm::ortho(0.0f, static_cast<float>(this->Width),
	               static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	glm::vec2 ballPos =
	    playerPos +
	    glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
	                      ResourceManager::GetTexture("face"));
}

void Game::ResetLevel() {
	for (auto &i : this->Levels[this->Level].Bricks) {
		i.Destroyed = false;
	}
}
void Game::ResetPlayer() {
	Player->Position = glm::vec2(this->Width / 2.0f - Player->Size.x / 2.0f,
	                             this->Height - Player->Size.y);
	Ball->Position =
	    glm::vec2(this->Width / 2.0f - Ball->Radius,
	              this->Height - Player->Size.y - 2 * Ball->Radius);
	Ball->Stuck = true;
}

void Game::Update(float dt) {
	Ball->Move(dt, this->Width);
	this->DoCollisions();
	if (Ball->Position.y >= this->Height) {
		this->ResetLevel();
		this->ResetPlayer();
	}
}

void Game::ProcessInput(float dt) {
	if (this->State == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_LEFT]) {
			if (Player->Position.x >= 0.0f) {
				Player->Position.x -= velocity;
				if (Ball->Stuck) {
					Ball->Position.x = Player->Position.x +
					                   Player->Size.x / 2.0f - Ball->Radius;
				}
			}
		}
		if (this->Keys[GLFW_KEY_RIGHT]) {
			if (Player->Position.x <= this->Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck) {
					Ball->Position.x = Player->Position.x +
					                   Player->Size.x / 2.0f - Ball->Radius;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = false;
		}
	}
}

void Game::Render() {
	if (this->State == GAME_ACTIVE) {
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
		                     glm::vec2(0.0f, 0.0f),
		                     glm::vec2(this->Width, this->Height), 0.0f);
		this->Levels[this->Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions() {
	for (GameObject &box : this->Levels[this->Level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) {
				if (!box.IsSolid) {
					box.Destroyed = true;
				}
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) {
					Ball->Velocity.x = -Ball->Velocity.x;
					float penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT) {
						Ball->Position.x += penetration;
					} else {

						Ball->Position.x -= penetration;
					}
				} else {

					Ball->Velocity.y = -Ball->Velocity.y;
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP) {
						Ball->Position.y -= penetration;
					} else {
						Ball->Position.y += penetration;
					}
				}
			}
		}
	}
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result)) {
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1 * fabs(Ball->Velocity.y);
		Ball->Velocity =
		    glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}

Collision CheckCollision(BallObject &one, GameObject &two) {
	glm::vec2 center(one.Position + one.Radius);
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x,
	                      two.Position.y + aabb_half_extents.y);
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped =
	    glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;
	if (glm::length(difference) <= one.Radius) {
		return std::make_tuple(true, VectorDirection(difference), difference);
	} else {
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
	}
}

Direction VectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
	    glm::vec2(0.0f, 1.0f),  // UP
	    glm::vec2(1.0f, 0.0f),  // Right
	    glm::vec2(0.0f, -1.0f), // DOWN
	    glm::vec2(-1.0f, 0.0f)  // LEFT
	};

	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++) {
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
