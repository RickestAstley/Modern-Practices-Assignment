
#include "collision.h"

Rectangle getProjectileRect(const Projectile& projectile) {
	return {
		projectile.position.x - 4.0f,
		std::min(projectile.lineStart.y, projectile.lineEnd.y),
		8.0f,
		std::abs(projectile.lineStart.y - projectile.lineEnd.y)
	};
}

void registerAlienHit(std::vector<Alien>& aliens, Projectile& projectile, int& score) {

	for (auto& alien : aliens)
	{
		Rectangle projectileRect = getProjectileRect(projectile);
		
		if (CheckCollisionCircleRec(alien.position, alien.radius, projectileRect))
		{
			projectile.active = false;
			alien.active = false;
			score += 100;
		}
	}
}

void registerWallHit(std::vector<Wall>& walls, Projectile& projectile) {
	for (auto& wall : walls)
	{
		Rectangle projectileRect = getProjectileRect(projectile);
		if (CheckCollisionCircleRec(wall.position, wall.radius, projectileRect))
		{
			projectile.active = false;
			wall.health -= 1;
		}
	}
}

void registerPlayerHit(Player& player, Projectile& projectile) {

	const Vector2 playerPos = { player.position.x, static_cast<float>(GetScreenHeight() - player.player_base_height) };
	Rectangle projectileRect = getProjectileRect(projectile);
	if (CheckCollisionCircleRec(playerPos, player.radius, projectileRect))
	{
		projectile.active = false;
		player.lives -= 1;
	}
}