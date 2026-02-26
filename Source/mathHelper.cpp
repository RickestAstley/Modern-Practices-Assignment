#include "mathHelper.h"

std::mt19937& GetRNG()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}