#include "game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

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

void Game::Start()
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
    gameState = State::GAMEPLAY;
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

void Game::Launch()
{
    // LOAD SOME RESOURCES HERE
    resources.Load();
}

void Game::Update()
{
    switch (gameState)
    {
    case State::STARTSCREEN:
        if (IsKeyReleased(KEY_SPACE))
        {
            Start();
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
    screen.playerPos = { player.x_pos, static_cast<float>(player.player_base_height) };
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
        
        projectiles.emplace_back(Vector2{player.x_pos, window_height -130.0f}, EntityType::PLAYER_PROJECTILE);
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
            // Check player projectile vs aliens
            for (auto& alien : aliens)
            {
                if (CheckCollision(alien.position, alien.radius, projectile.lineStart, projectile.lineEnd))
                {
                    std::cout << "Hit!\n";
                    projectile.active = false;
                    alien.active = false;
                    score += 100;
                }
            }

            // Check player projectile vs walls
            for (auto& wall : walls)
            {
                if (CheckCollision(wall.position, wall.radius, projectile.lineStart, projectile.lineEnd))
                {
                    std::cout << "Hit!\n";
                    projectile.active = false;
                    wall.health -= 1;
                }
            }
        }
        else if (projectile.type == EntityType::ENEMY_PROJECTILE)
        {
            // Check enemy projectile vs player
            const Vector2 playerPos = { player.x_pos, static_cast<float>(GetScreenHeight() - player.player_base_height) };
            if (CheckCollision(playerPos, player.radius, projectile.lineStart, projectile.lineEnd))
            {
                std::cout << "Player hit!\n";
                projectile.active = false;
                player.lives -= 1;
            }

            // Check enemy projectile vs walls
            for (auto& wall : walls)
            {
                if (CheckCollision(wall.position, wall.radius, projectile.lineStart, projectile.lineEnd))
                {
                    projectile.active = false;
                    wall.health -= 1;
                }
            }
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

    if (textInput.mouseOnText)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        // Get char pressed on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0)
        {
            // Only allow keys in range [32..125] (printable ASCII)
            // Ensure we have room for the character AND null terminator
            constexpr int maxNameLength = 8;
            if ((key >= 32) && (key <= 125) && (textInput.letterCount < maxNameLength))
            {
                textInput.name[textInput.letterCount] = static_cast<char>(key);
                ++textInput.letterCount;
                
                // Safely add null terminator
                if (textInput.letterCount < textInput.name.size())
                {
                    textInput.name[textInput.letterCount] = '\0';
                }
            }
            key = GetCharPressed();
        }

        // Remove chars
        if (IsKeyPressed(KEY_BACKSPACE) && textInput.letterCount > 0)
        {
            --textInput.letterCount;
            textInput.name[textInput.letterCount] = '\0';
        }

        ++textInput.framesCounter;
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        textInput.framesCounter = 0;
    }

    // If the name is right length and enter is pressed
    constexpr int maxNameLength = 8;
    if (textInput.letterCount > 0 && textInput.letterCount <= maxNameLength && IsKeyReleased(KEY_ENTER))
    {
        const std::string nameEntry(textInput.name.data());
        InsertNewHighScore(nameEntry);
        newHighScore = false;
        
        // Reset text input
        textInput.letterCount = 0;
        textInput.name.fill('\0');
    }
}

void Game::Render()
{
    switch (gameState)
    {
    case State::STARTSCREEN:
        RenderStartScreen();
        break;

    case State::GAMEPLAY:
        RenderGameplay();
        break;

    case State::ENDSCREEN:
        RenderEndScreen();
        break;
    }
}

void Game::RenderStartScreen()
{
    DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);
    DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);
}

void Game::RenderGameplay()
{
    // background render LEAVE THIS AT TOP
    background.Render();

    DrawText(TextFormat("Score: %i", score), 50, 20, 40, YELLOW);
    DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

    // player rendering
    player.Render(resources.shipTextures[player.activeTexture]);

    // Use range-based for loops for rendering
    for (const auto& projectile : projectiles)
    {
        projectile.Render(resources.laserTexture);
    }

    for (const auto& wall : walls)
    {
        wall.Render(resources.barrierTexture);
    }

    for (const auto& alien : aliens)
    {
        alien.Render(resources.alienTexture);
    }
}

void Game::RenderEndScreen()
{
    if (newHighScore)
    {
        RenderNewHighScore();
    }
    else
    {
        RenderLeaderboard();
    }
}

void Game::RenderNewHighScore()
{
    DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);
    DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

    DrawRectangleRec(textInput.textBox, LIGHTGRAY);
    
    const Color borderColor = textInput.mouseOnText ? RED : DARKGRAY;
    DrawRectangleLines(
        static_cast<int>(textInput.textBox.x),
        static_cast<int>(textInput.textBox.y),
        static_cast<int>(textInput.textBox.width),
        static_cast<int>(textInput.textBox.height),
        borderColor
    );

    DrawText(textInput.name.data(), static_cast<int>(textInput.textBox.x) + 5, static_cast<int>(textInput.textBox.y) + 8, 40, MAROON);
    DrawText(TextFormat("INPUT CHARS: %i/%i", textInput.letterCount, 8), 600, 600, 20, YELLOW);

    if (textInput.mouseOnText)
    {
        if (textInput.letterCount < 9)
        {
            // Draw blinking underscore char
            if (((textInput.framesCounter / 20) % 2) == 0)
            {
                DrawText("_",
                    static_cast<int>(textInput.textBox.x) + 8 + MeasureText(textInput.name.data(), 40),
                    static_cast<int>(textInput.textBox.y) + 12,
                    40,
                    MAROON
                );
            }
        }
        else
        {
            DrawText("Press BACKSPACE to delete chars...", 600, 650, 20, YELLOW);
        }
    }

    if (textInput.letterCount > 0 && textInput.letterCount < 9)
    {
        DrawText("PRESS ENTER TO CONTINUE", 600, 800, 40, YELLOW);
    }
}

void Game::RenderLeaderboard()
{
    DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);
    DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

    for (size_t i = 0; i < leaderboard.size(); ++i)
    {
        const char* nameDisplay = leaderboard[i].name.c_str();
        const int yPos = 140 + static_cast<int>(i * 40);
        DrawText(nameDisplay, 50, yPos, 40, YELLOW);
        DrawText(TextFormat("%i", leaderboard[i].score), 350, yPos, 40, YELLOW);
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

[[nodiscard]] bool Game::CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) const noexcept
{
    // Check if either edge of line is within circle
    if (pointInCircle(circlePos, circleRadius, lineStart) || pointInCircle(circlePos, circleRadius, lineEnd))
    {
        return true;
    }

    // Calculate the length of the line
    const float length = lineLength(lineStart, lineEnd);

    if (length < 0.001f) // Avoid division by zero
    {
        return false;
    }

    // Calculate the dot product
    const float dx = lineEnd.x - lineStart.x;
    const float dy = lineEnd.y - lineStart.y;
    const float dotP = ((circlePos.x - lineStart.x) * dx + (circlePos.y - lineStart.y) * dy) / (length * length);

    // Use dot product to find closest point
    const float closestX = lineStart.x + (dotP * dx);
    const float closestY = lineStart.y + (dotP * dy);

    // Check if point is on the line segment
    constexpr float buffer = 0.1f;
    const float closeToStart = lineLength(lineStart, { closestX, closestY });
    const float closeToEnd = lineLength(lineEnd, { closestX, closestY });
    const float closestLength = closeToStart + closeToEnd;

    if (std::abs(closestLength - length) <= buffer)
    {
        // Point is on the line - check distance to circle center
        const float closeToCentre = lineLength(circlePos, { closestX, closestY });
        return closeToCentre < circleRadius;
    }

    return false;
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

    x_pos += speed * direction;

    // Clamp position to screen bounds
    x_pos = std::clamp(x_pos, radius, static_cast<float>(GetScreenWidth()) - radius);

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
        { x_pos, window_height - player_base_height, 100.0f, 100.0f },
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

