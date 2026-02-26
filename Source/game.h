#pragma once
#include "raylib.h"
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include "Resources.h"
#include "entities.h"

enum struct State
{
    STARTSCREEN,
    GAMEPLAY,
    ENDSCREEN
};

struct PlayerData
{
    std::string name;
    int score = 0;
};

struct Game
{

	Game();

    // Gamestate
    State gameState = State::STARTSCREEN;

    // Score
    int score = 0;

    // Configuration
    int wallCount = 5;

    // Aliens shooting
    float shootTimer = 0.0f;

    // Alien formation
    int formationWidth = 8;
    int formationHeight = 5;
    int alienSpacing = 80;
    Vector2 formationStart{ 100.0f, 50.0f };

    bool newHighScore = false;

    // Main game loop functions
    
    void End();
    void Continue();
    void Update();
    void Render();

    // Gameplay functions
    void SpawnAliens();
    void UpdateGameplay();
    void UpdateEndScreen();
    void UpdateTextInput();
    void CheckAllCollisions();
    void HandleAlienShooting();

	// Utility functions
    [[nodiscard]] bool CheckNewHighScore() const noexcept;
    void InsertNewHighScore(std::string_view name);

    // Entity Storage and Resources
    Resources resources;
    Player player;

    std::vector<Projectile> projectiles;
    std::vector<Wall> walls;
    std::vector<Alien> aliens;
    std::vector<PlayerData> leaderboard = {
        {"Player 1", 500},
        {"Player 2", 400},
        {"Player 3", 300},
        {"Player 4", 200},
        {"Player 5", 100}
    };

    Background background;

    // Screen data
    struct ScreenData
    {
        Vector2 playerPos{};
        Vector2 alienPos{};
        Vector2 cornerPos{};
        float offset = 0.0f;
    } screen;
};

struct TextInputState
{
	std::string name;
	Rectangle textBox{ 600.0f, 500.0f, 225.0f, 50.0f };
	bool mouseOnText = false;
	int framesCounter = 0;
	const int maxNameLength = 8;
};