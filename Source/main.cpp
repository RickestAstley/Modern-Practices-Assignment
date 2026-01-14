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
*b
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

const int screenWidth = 1920;
const int screenHeight = 1080;
const int TARGET_FPS = 60;


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{    
    // Initialization
    //--------------------------------------------------------------------------------------
    
	WindowContext window(screenWidth, screenHeight, "Space Invaders", TARGET_FPS);


    Game game;
    game.gameState = { State::STARTSCREEN };
    game.Launch();
    //--------------------------------------------------------------------------------------

    InitAudioDevice();

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        game.Update();
        BeginDrawing();
        ClearBackground(BLACK);
        game.Render();
        EndDrawing();

    }

    return 0;
}