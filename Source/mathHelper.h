#pragma once
#include <algorithm>
#include <random>
#include "raylib.h"

std::mt19937& GetRNG();

[[nodiscard]] float lineLength(Vector2 A, Vector2 B) noexcept;

[[nodiscard]] bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept;