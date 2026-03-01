#pragma once
#include "raylib.h"
#include "game.h"
#include "Resources.h"

class WindowContext
{
public:
    WindowContext(int width, int height, const char* title, int fps)
    {
        InitWindow(width, height, title);
        SetTargetFPS(fps);
    }

    WindowContext(const WindowContext&) = delete;
    WindowContext(WindowContext&&) = delete;
    WindowContext& operator=(const WindowContext&) = delete;
    WindowContext& operator=(WindowContext&&) = delete; 

    ~WindowContext()
    {
        CloseWindow();
    }
};

class DrawingContext
{
public:
    DrawingContext()
    {
        BeginDrawing();
    }

    DrawingContext(const DrawingContext&) = delete;
    DrawingContext(DrawingContext&&) = delete;
    DrawingContext& operator=(const DrawingContext&) = delete;
    DrawingContext& operator=(DrawingContext&&) = delete;

    ~DrawingContext()
    {
        EndDrawing();
    }
};

void RenderStartScreen();

void RenderGameplay(Background& background, int score, Resources& resources,
	std::vector<Projectile>& projectiles, std::vector<Wall>& walls, std::vector<Alien>& aliens, Player& player);

void RenderEndScreen(bool newHighScore, TextInputState textInput, std::vector<PlayerData> leaderboard);

void RenderNewHighScore(TextInputState textInput);

void RenderLeaderboard(std::vector<PlayerData> leaderboard);