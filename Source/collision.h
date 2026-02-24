#pragma once

#include "game.h"

void registerAlienHit(std::vector<Alien>& aliens, Projectile& projectile, int& score);

void registerWallHit(std::vector<Wall>& walls, Projectile& projectile);

void registerPlayerHit(Player& player, Projectile& projectile);