#include "mathHelper.h"

std::mt19937& GetRNG()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

[[nodiscard]] float lineLength(Vector2 A, Vector2 B) noexcept
{
	const float dx = B.x - A.x;
	const float dy = B.y - A.y;
	return std::sqrt(dx * dx + dy * dy);
}

[[nodiscard]] bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept
{
	return lineLength(circlePos, point) < radius;
}