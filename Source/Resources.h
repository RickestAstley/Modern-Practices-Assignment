#pragma once
#include <vector>
#include <array>
#include "raylib.h"

struct Resources 
{
       
    Resources();

	~Resources();

	Resources(const Resources&) = delete;

	Resources& operator=(const Resources&) = delete;

	Resources(Resources&& other) noexcept;

	Resources& operator=(Resources&& other) noexcept;

    // Using std::array for fixed-size collections
    std::array<Texture2D, 3> shipTextures;
    Texture2D alienTexture{};
    Texture2D barrierTexture{};
    Texture2D laserTexture{};
};