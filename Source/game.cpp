#include "game.h"
#include "collision.h"
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include "render.h"

// Modern random number generator (thread-safe)
namespace {
    std::mt19937& GetRNG()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
}

// MATH FUNCTIONS - modernized with inline and noexcept
[[nodiscard]] inline float lineLength(Vector2 A, Vector2 B) noexcept
{
    const float dx = B.x - A.x;
    const float dy = B.y - A.y;
    return std::sqrt(dx * dx + dy * dy);
}

[[nodiscard]] inline bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept
{
    return lineLength(circlePos, point) < radius;
}

Game::Game()
{
    // creating walls 
    const float window_width = static_cast<float>(GetScreenWidth());
    const float window_height = static_cast<float>(GetScreenHeight());
    const float wall_distance = window_width / (wallCount + 1);
    
    walls.clear();
    walls.reserve(wallCount); // Pre-allocate memory
    
    for (int i = 0; i < wallCount; ++i)
    {
		walls.emplace_back(Vector2{ wall_distance * (i + 1), window_height - 250.0f });
    }

    // creating player
    player = Player{};

    // creating aliens
    SpawnAliens();

    // creating background
    background = Background(600);

    // reset score
    score = 0;
    gameState = State::STARTSCREEN;
}

void Game::End()
{
    // SAVE SCORE AND UPDATE SCOREBOARD
    projectiles.clear();
    walls.clear();
    aliens.clear();
    newHighScore = CheckNewHighScore();
    gameState = State::ENDSCREEN;
}

void Game::Continue()
{
    gameState = State::STARTSCREEN;
}

void Game::Update()
{
    switch (gameState)
    {
    case State::STARTSCREEN:
        if (IsKeyReleased(KEY_SPACE))
        {
			gameState = State::GAMEPLAY;
        }
        break;

    case State::GAMEPLAY:
        UpdateGameplay();
        break;

    case State::ENDSCREEN:
        UpdateEndScreen();
        break;
    }
}

void Game::UpdateGameplay()
{
    if (IsKeyReleased(KEY_Q))
    {
        End();
        return;
    }

    // Update Player
    player.Update();

    // Update Aliens and Check if they are past player
    for (auto& alien : aliens)
    {
        alien.Update();
        if (alien.position.y > GetScreenHeight() - player.player_base_height)
        {
            End();
            return;
        }
    }

    // End game if player dies
    if (player.lives < 1)
    {
        End();
        return;
    }

    // Spawn new aliens if aliens run out
    if (aliens.empty())
    {
        SpawnAliens();
    }

    // Update background with offset
    screen.playerPos = { player.position.x, static_cast<float>(player.player_base_height) };
    screen.cornerPos = { 0.0f, static_cast<float>(player.player_base_height) };
    screen.offset = lineLength(screen.playerPos, screen.cornerPos) * -1.0f;
    background.Update(screen.offset / 15.0f);

    // Update projectiles and walls using range-based loops
    for (auto& projectile : projectiles)
    {
        projectile.Update();
    }

    for (auto& wall : walls)
    {
        wall.Update();
    }

    // Check collisions
    CheckAllCollisions();

    // Handle player shooting
    if (IsKeyPressed(KEY_SPACE))
    {
        const float window_height = static_cast<float>(GetScreenHeight());
        
        projectiles.emplace_back(Vector2{player.position.x, window_height -130.0f}, EntityType::PLAYER_PROJECTILE);
    }

    // Aliens Shooting
    HandleAlienShooting();

    // REMOVE INACTIVE/DEAD ENTITIES using C++20 std::erase_if
    std::erase_if(projectiles, [](const auto& p) { return !p.active; });
    std::erase_if(aliens, [](const auto& a) { return !a.active; });
    std::erase_if(walls, [](const auto& w) { return !w.active; });
}

void Game::CheckAllCollisions()
{
    for (auto& projectile : projectiles)
    {

        if (projectile.type == EntityType::PLAYER_PROJECTILE)
        {
            
		    registerAlienHit(aliens, projectile, score);
            

            // Check player projectile vs walls
			registerWallHit(walls, projectile);

        }
        else if (projectile.type == EntityType::ENEMY_PROJECTILE)
        {
            // Check enemy projectile vs player
			registerPlayerHit(player, projectile);

            // Check enemy projectile vs walls
			registerWallHit(walls, projectile);
        }
    }
}


void Game::HandleAlienShooting()
{
    shootTimer += 1.0f;
    if (shootTimer > 59.0f && !aliens.empty()) // once per second
    {
        std::uniform_int_distribution<size_t> dist(0, aliens.size() - 1);
        const size_t randomAlienIndex = dist(GetRNG());

        Vector2 pos = aliens[randomAlienIndex].position;
        pos.y += 40.0f;
        projectiles.emplace_back(pos, EntityType::ENEMY_PROJECTILE, -15);
        shootTimer = 0.0f;
    }
}

void Game::UpdateEndScreen()
{
    // Exit endscreen
    if (IsKeyReleased(KEY_ENTER) && !newHighScore)
    {
        Continue();
        return;
    }

    if (newHighScore)
    {
        UpdateTextInput();
    }
}

void Game::UpdateTextInput()
{
	textInput.mouseOnText = CheckCollisionPointRec(GetMousePosition(), textInput.textBox);
	SetMouseCursor(textInput.mouseOnText ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);

	if (!textInput.mouseOnText) {
		textInput.framesCounter = 0;
		return;
	}

	// Get char pressed on the queue
	int key = GetCharPressed();

	// Check if more characters have been pressed on the same frame
	while (key > 0)
	{
		if ((key >= 32) && (key <= 125) && (textInput.name.length() < textInput.maxNameLength))
		{
			textInput.name.push_back(static_cast<char>(key));
		}
		key = GetCharPressed();
	}

	// Remove chars
	if (IsKeyPressed(KEY_BACKSPACE) && !textInput.name.empty())
	{
		textInput.name.pop_back();
	}

	if (!textInput.name.empty() && IsKeyReleased(KEY_ENTER))
	{
		InsertNewHighScore(textInput.name);
		newHighScore = false;
		textInput.name.clear();
	}

	++textInput.framesCounter;

}

void Game::Render()
{
    switch (gameState)
    {
    case State::STARTSCREEN:
        RenderStartScreen();
        break;

    case State::GAMEPLAY:
        RenderGameplay(background, score, resources, projectiles, walls, aliens, player);
        break;

    case State::ENDSCREEN:
        RenderEndScreen(newHighScore, textInput, leaderboard);
        break;
    }
}

void Game::SpawnAliens()
{
    aliens.clear();
    aliens.reserve(formationWidth * formationHeight); // Pre-allocate memory

    for (int row = 0; row < formationHeight; ++row)
    {
        for (int col = 0; col < formationWidth; ++col)
        {
            float x = formationStart.x + 450.0f + (col * alienSpacing);
            float y = formationStart.y + (row * alienSpacing);
            aliens.emplace_back(Vector2{ x, y });
        }
    }
}

[[nodiscard]] bool Game::CheckNewHighScore() const noexcept
{
    return !leaderboard.empty() && score > leaderboard.back().score;
}

void Game::InsertNewHighScore(std::string_view name)
{
    PlayerData newData{ std::string(name), score };

    // Find insertion position using algorithm
    auto it = std::find_if(leaderboard.begin(), leaderboard.end(),
        [&newData](const PlayerData& pd) { return newData.score > pd.score; });

    if (it != leaderboard.end())
    {
        leaderboard.insert(it, newData);
        
        // Keep only top 5
        if (leaderboard.size() > 5)
        {
            leaderboard.pop_back();
        }
    }
}


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

