
#include "collision.h"

void registerHit(Alien& entity, Projectile& projectile, int& score) {

    Rectangle projectileRect = {
            projectile.position.x - 2.0f,
            std::min(projectile.lineStart.y, projectile.lineEnd.y),
            8.0f,
            std::abs(projectile.lineStart.y - projectile.lineEnd.y)
    };

    if (CheckCollisionCircleRec(entity.position, entity.radius, projectileRect))
    {
        projectile.active = false;
        entity.active = false;
        score += 100;
    }
}