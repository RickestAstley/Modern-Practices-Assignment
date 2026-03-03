#pragma once
#include "raylib.h"
#include <vector>
#include "mathHelper.h"

enum struct EntityType
{
	PLAYER,
	ENEMY,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
};


struct Entity
{
	Vector2 position{ 0.0f, 0.0f };
	float radius = 0.0f;
	bool active = true;
	EntityType type{};
	virtual ~Entity() = default;
};

struct Player : public Entity
{
	float speed = 7.0f;
	float player_base_height = 70.0f;
	int lives = 3;
	int direction = 0;
	int activeTexture = 0;
	float timer = 0.0f;

	EntityType type = EntityType::PLAYER;

	Player(float x = static_cast<float>(GetScreenWidth()) / 2)
	{
		position = { x, player_base_height };
		radius = 50.0f;
	}

	void Render(Texture2D texture) const;
	void Update();

};

struct Wall : public Entity
{
	Rectangle rec{};
	Color color = WHITE;
	int health = 50;

	Wall(Vector2 pos) {
		position = pos;
		radius = 60.0f;
	}

	void Render(Texture2D texture) const;
	void Update();
};

struct Alien : public Entity
{
	Color color = WHITE;
	bool moveRight = true;

	EntityType type = EntityType::ENEMY;

	int speed = 2;

	Alien(Vector2 pos) {

		position = pos;
		radius = 40.0f;
	}

	void Update();
	void Render(Texture2D texture) const;
};

struct Projectile
{
	Vector2 position{ 0.0f, 0.0f };
	int speed = 15;
	bool active = true;
	EntityType type{};

	Vector2 lineStart{ 0.0f, 0.0f };
	Vector2 lineEnd{ 0.0f, 0.0f };

	Projectile(Vector2 pos, EntityType t, int sp = 15) : position(pos), type(t), speed(sp)
	{
		lineStart = { position.x, position.y - 15.0f };
		lineEnd = { position.x, position.y + 15.0f };
	}

	void Update();
	void Render(Texture2D texture) const;
};

struct Star
{
	Vector2 initPosition{ 0.0f, 0.0f };
	Vector2 position{ 0.0f, 0.0f };
	Color color = GRAY;
	float size = 0.0f;

	Star(Vector2 pos, float s) : initPosition(pos), position(pos), size(s) {}

	void Update(float starOffset);
	void Render() const;
};

struct Background
{
	std::vector<Star> Stars;

	Background(int starAmount);
	void Update(float offset);
	void Render() const;
};