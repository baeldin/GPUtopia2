#include "color.h"

float clamp(const float x) { return (x < 0.f) ? 0.f : (x > 1.f) ? 1.f : x; }
color clamp(const color& c) { return color(clamp(c.r), clamp(c.g), clamp(c.b), clamp(c.a)); }

constexpr float a = 1.f / 12.92f;
constexpr float b = 1.f / 1.055f;
inline float sRGBtoLinear(const float x) {
	return (clamp(x) <= 0.04045f) ? clamp(x) * a : std::pow((clamp(x) + 0.055f) * b, 2.4f);
}

constexpr float invGamma = 1.f / 2.4f;
inline float linearToSRGB(const float x) {
	return (clamp(x) <= 0.0031308f) ? clamp(x) * 12.92f : 1.055f * std::pow(clamp(x), invGamma) - 0.055f;
}

inline color sRGBtoLinear(const color& c)
{
	return color(sRGBtoLinear(c.r), sRGBtoLinear(c.g), sRGBtoLinear(c.b), c.a);
}
inline color linearToSRGB(const color& c)
{
	return color(linearToSRGB(c.r), linearToSRGB(c.g), linearToSRGB(c.b), c.a);
}