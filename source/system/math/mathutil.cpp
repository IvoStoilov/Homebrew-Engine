#include "system/math/mathutil.h"
#include <algorithm>

uint32_t MathUtil::Clamp(const uint32_t minV, const uint32_t maxV, uint32_t x)
{
	return std::max(std::min(x, maxV), minV);
}

float MathUtil::Clamp(const float minV, const float maxV, const float x)
{
	return std::max(std::min(x, maxV), minV);
}