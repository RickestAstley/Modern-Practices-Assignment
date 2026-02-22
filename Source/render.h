#pragma once
#include "raylib.h"

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