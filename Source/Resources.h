#pragma once
#include <vector>
#include <array>
#include "raylib.h"

struct Resources 
{
    void Load();
    
    // Using std::array for fixed-size collections
    std::array<Texture2D, 3> shipTextures;
    Texture2D alienTexture{};
    Texture2D barrierTexture{};
    Texture2D laserTexture{};
    
    // Add destructor to unload textures (RAII principle)
    ~Resources();
    
    // Prevent copying (textures shouldn't be copied)
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;
    
    // Allow moving if needed
    Resources(Resources&&) = default;
    Resources& operator=(Resources&&) = default;
    Resources() = default;
};