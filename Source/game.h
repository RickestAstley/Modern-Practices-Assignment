#pragma once
#include "raylib.h"
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include "Resources.h"

enum struct State
{
    STARTSCREEN,
    GAMEPLAY,
    ENDSCREEN
};

enum struct EntityType
{
    PLAYER,
    ENEMY,
    PLAYER_PROJECTILE,
    ENEMY_PROJECTILE
};

struct PlayerData
{
    std::string name;
    int score = 0;
};

struct Player
{
    float x_pos = 0.0f;
    float speed = 7.0f;
    float player_base_height = 70.0f;
    float radius = 50.0f;
    int lives = 3;
    int direction = 0;
    int activeTexture = 0;
    float timer = 0.0f;

    EntityType type = EntityType::PLAYER;

    Player(float x = static_cast<float>(GetScreenWidth())/ 2) : x_pos(x), speed(7.0f), player_base_height(70.0f), radius(50.0f), lives(3), direction(0), activeTexture(0), timer(0.0f)
	{
	}

	~Player() = default;

    void Render(Texture2D texture) const;
    void Update();

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

struct Wall
{
    Vector2 position{ 0.0f, 0.0f };
    Rectangle rec{};
    bool active = true;
    Color color = WHITE;
    int health = 50;
    float radius = 60.0f;

    Wall(Vector2 pos) : position(pos) {}

    void Render(Texture2D texture) const;
    void Update();
};

struct Alien
{
    Color color = WHITE;
    Vector2 position{ 0.0f, 0.0f };
    float radius = 30.0f;
    bool active = true;
    bool moveRight = true;

    EntityType type = EntityType::ENEMY;

    int speed = 2;

	Alien(Vector2 pos) : position(pos) {}

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

	Background() = default;
    Background(int starAmount);
    void Update(float offset);
    void Render() const;
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

    // Render functions
    void RenderStartScreen();
    void RenderGameplay();
    void RenderEndScreen();
    void RenderNewHighScore();
    void RenderLeaderboard();

    // Utility functions
    [[nodiscard]] bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) const noexcept;
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

    // Text input state
    struct TextInputState
    {
        std::array<char, 10> name{ '\0' };
        int letterCount = 0;
        Rectangle textBox{ 600.0f, 500.0f, 225.0f, 50.0f };
        bool mouseOnText = false;
        int framesCounter = 0;
    } textInput;
};