#include "entities.h"


void Player::Update()
{
	// Movement
	direction = 0;
	if (IsKeyDown(KEY_LEFT))
	{
		--direction;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		++direction;
	}

	position.x += speed * direction;

	// Clamp position to screen bounds
	position.x = std::clamp(position.x, radius, static_cast<float>(GetScreenWidth()) - radius);

	// Determine frame for animation
	timer += GetFrameTime();

	if (timer > 0.4f)
	{
		activeTexture = (activeTexture + 1) % 3;
		timer = 0.0f;
	}
}

void Player::Render(Texture2D texture) const
{
	const float window_height = static_cast<float>(GetScreenHeight());

	DrawTexturePro(
		texture,
		{ 0.0f, 0.0f, 352.0f, 352.0f },
		{ position.x, window_height - player_base_height, 100.0f, 100.0f },
		{ 50.0f, 50.0f },
		0.0f,
		WHITE
	);
}

// PROJECTILE IMPLEMENTATION
void Projectile::Update()
{
	position.y -= speed;

	// UPDATE LINE POSITION
	lineStart = { position.x, position.y - 15.0f };
	lineEnd = { position.x, position.y + 15.0f };

	if (position.y < 0.0f || position.y > 1500.0f)
	{
		active = false;
	}
}

void Projectile::Render(Texture2D texture) const
{
	DrawTexturePro(
		texture,
		{ 0.0f, 0.0f, 176.0f, 176.0f },
		{ position.x, position.y, 50.0f, 50.0f },
		{ 25.0f, 25.0f },
		0.0f,
		WHITE
	);
}

// WALL IMPLEMENTATION
void Wall::Render(Texture2D texture) const
{
	DrawTexturePro(
		texture,
		{ 0.0f, 0.0f, 704.0f, 704.0f },
		{ position.x, position.y, 200.0f, 200.0f },
		{ 100.0f, 100.0f },
		0.0f,
		WHITE
	);

	DrawText(TextFormat("%i", health), static_cast<int>(position.x) - 21, static_cast<int>(position.y) + 10, 40, RED);
}

void Wall::Update()
{
	if (health < 1)
	{
		active = false;
	}
}

// ALIEN IMPLEMENTATION
void Alien::Update()
{
	const int window_width = GetScreenWidth();

	if (moveRight)
	{
		position.x += speed;

		if (position.x >= window_width)
		{
			moveRight = false;
			position.y += 50.0f;
		}
	}
	else
	{
		position.x -= speed;

		if (position.x <= 0.0f)
		{
			moveRight = true;
			position.y += 50.0f;
		}
	}
}

void Alien::Render(Texture2D texture) const
{
	DrawTexturePro(
		texture,
		{ 0.0f, 0.0f, 352.0f, 352.0f },
		{ position.x, position.y, 100.0f, 100.0f },
		{ 50.0f, 50.0f },
		0.0f,
		WHITE
	);
}

// BACKGROUND IMPLEMENTATION
void Star::Update(float starOffset)
{
	position.x = initPosition.x + starOffset;
	position.y = initPosition.y;
}

void Star::Render() const
{
	DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), size, color);
}

Background::Background(int starAmount)
{
	Stars.clear();
	Stars.reserve(starAmount);

	std::uniform_int_distribution<int> distX(-150, GetScreenWidth() + 150);
	std::uniform_int_distribution<int> distY(0, GetScreenHeight());
	std::uniform_int_distribution<int> distSize(1, 4);

	for (int i = 0; i < starAmount; ++i)
	{
		Vector2 pos = { static_cast<float>(distX(GetRNG())), static_cast<float>(distY(GetRNG())) };
		float size = static_cast<float>(distSize(GetRNG())) / 2.0f;
		Stars.emplace_back(pos, size);
	}
}

void Background::Update(float offset)
{
	for (auto& star : Stars)
	{
		star.Update(offset);
	}
}

void Background::Render() const
{
	for (const auto& star : Stars)
	{
		star.Render();
	}
}