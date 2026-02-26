#include "game.h"
#include "collision.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"


Game::Game()
	: player(), background(600), score(0), gameState(State::STARTSCREEN)
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

    // creating aliens
    SpawnAliens();
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
	float playerOffset = -player.position.x;
	background.Update(playerOffset / 15.0f);

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

		if (!projectile.active) continue;

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
        [this](const PlayerData& pd) { return score > pd.score; });

    if (it != leaderboard.end())
    {
		leaderboard.emplace(it, std::string(name), score);
        
        // Keep only top 5
        if (leaderboard.size() > 5)
        {
            leaderboard.pop_back();
        }
    }
}

