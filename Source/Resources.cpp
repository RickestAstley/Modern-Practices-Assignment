#include "Resources.h"
#include <iostream>

Resources::Resources()
{
    std::cout << "Loading resources...\n"; // Improved message
    
    auto loadTextureChecked = [](const std::string& path) -> Texture2D {
        Texture2D tex = LoadTexture(path.c_str());
        if (tex.id == 0) {
            throw std::runtime_error("Failed to load texture: " + path);
        }
        return tex;
    };

    alienTexture = loadTextureChecked("./Assets/Alien.png");
    barrierTexture = loadTextureChecked("./Assets/Barrier.png");
    laserTexture = loadTextureChecked("./Assets/Laser.png");

    shipTextures[0] = loadTextureChecked("./Assets/Ship1.png");
    shipTextures[1] = loadTextureChecked("./Assets/Ship2.png");
    shipTextures[2] = loadTextureChecked("./Assets/Ship3.png");
}

Resources::~Resources()
{
    // RAII: Clean up textures
    for (auto& texture : shipTextures)
    {
        UnloadTexture(texture);
    }
    UnloadTexture(alienTexture);
    UnloadTexture(barrierTexture);
    UnloadTexture(laserTexture);
}