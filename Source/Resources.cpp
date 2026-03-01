#include "Resources.h"
#include <iostream>

Resources::Resources()
    :alienTexture("./Assets/Alien.png"),
     barrierTexture("./Assets/Barrier.png"),
	laserTexture("./Assets/Laser.png")
{

    shipTextures[0] = Texture("./Assets/Ship1.png");
    shipTextures[1] = Texture("./Assets/Ship2.png");
    shipTextures[2] = Texture("./Assets/Ship3.png");
}