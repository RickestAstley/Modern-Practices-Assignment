/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "game.h"

class WindowContext
{
public:
    WindowContext(int width, int height, const char* title, int fps)
    {
        InitWindow(width, height, title);
        SetTargetFPS(fps);
    }

    WindowContext(const WindowContext&) = delete;
    WindowContext& operator=(const WindowContext&) = delete;

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
    DrawingContext& operator=(const DrawingContext&) = delete;

    ~DrawingContext()
    {
        EndDrawing();
    }
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;
    constexpr int targetFPS = 60;

    WindowContext window(screenWidth, screenHeight, "Space Invaders", targetFPS);

    Game game;
    game.Launch();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        game.Update();
        
        {
            DrawingContext drawing;
            ClearBackground(BLACK);
            game.Render();
        }
    }

    return 0;
}