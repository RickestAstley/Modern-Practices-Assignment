#include "Resources.h"
#include <iostream>

void Resources::Load()
{
    std::cout << "Loading resources...\n"; // Improved message
    
    alienTexture = LoadTexture("./Assets/Alien.png");
    barrierTexture = LoadTexture("./Assets/Barrier.png");
    laserTexture = LoadTexture("./Assets/Laser.png");
    
    // Use array initialization instead of push_back
    if (shipTextures.size() != 3) {
		throw std::runtime_error("shipTextures array size is incorrect.");
	} // JUST IN CASE THIS SOMEHOW FAILS
    shipTextures[0] = LoadTexture("./Assets/Ship1.png");
    shipTextures[1] = LoadTexture("./Assets/Ship2.png");
    shipTextures[2] = LoadTexture("./Assets/Ship3.png");
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