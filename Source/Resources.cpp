#include "Resources.h"


Resources::Resources()
    :alienTexture("./Assets/Alien.png"),
     barrierTexture("./Assets/Barrier.png"),
	laserTexture("./Assets/Laser.png")
{

    shipTextures[0] = TextureWrapper("./Assets/Ship1.png");
    shipTextures[1] = TextureWrapper("./Assets/Ship2.png");
    shipTextures[2] = TextureWrapper("./Assets/Ship3.png");
}