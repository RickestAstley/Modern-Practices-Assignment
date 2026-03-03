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

#include "render.h"
#include "game.h"
#include <iostream>
#include <exception>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;
    constexpr int targetFPS = 60;

    try
    {


        WindowContext window(screenWidth, screenHeight, "Space Invaders", targetFPS);

        Game game;

        // Main game loop
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
            game.Update();
            DrawingContext drawing;
            ClearBackground(BLACK);
            game.Render();

        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
	}

    return 0;
}