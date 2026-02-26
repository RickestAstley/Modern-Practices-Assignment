#include "Resources.h"
#include <iostream>

Resources::Resources()
{
    std::cout << "Loading resources...\n";
    
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

Resources::Resources(Resources&& other) noexcept
	: alienTexture(std::exchange(other.alienTexture, { 0 })),
	barrierTexture(std::exchange(other.barrierTexture, { 0 })),
	laserTexture(std::exchange(other.laserTexture, { 0 })),
	shipTextures(other.shipTextures)
{
	other.shipTextures.fill({ 0 });
}

Resources& Resources::operator=(Resources&& other) noexcept
{
	if (this != &other)
	{
		UnloadTexture(alienTexture);
		UnloadTexture(barrierTexture);
		UnloadTexture(laserTexture);
		for (const auto& texture : shipTextures) UnloadTexture(texture);

		alienTexture = std::exchange(other.alienTexture, { 0 });
		barrierTexture = std::exchange(other.barrierTexture, { 0 });
		laserTexture = std::exchange(other.laserTexture, { 0 });

		shipTextures = other.shipTextures;
		other.shipTextures.fill({ 0 });
	}
	return *this;
}