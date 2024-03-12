// define some auxiliary functions for the use with colors, like gamma correction,
// clamping, etc...

float clampZeroToOne(const float x)
{
	return fmax(0.f, fmin(1.f, x));
}

const float a = 1.f / 12.92f;
const float b = 1.f / 1.055f;
float fsRGBtoLinear(const float x) {
	return (clampZeroToOne(x) <= 0.04045f) ? clampZeroToOne(x) * a : pow((clampZeroToOne(x) + 0.055f) * b, 2.4f);
}

const float invGamma = 1.f / 2.4f;
float flinearToSRGB(const float x) {
	return (clampZeroToOne(x) <= 0.0031308f) ? clampZeroToOne(x) * 12.92f : 1.055f * pow(clampZeroToOne(x), invGamma) - 0.055f;
}

float4 sRGBtoLinear(const float4 v) {
	return (float4)(fsRGBtoLinear(v.x), fsRGBtoLinear(v.y), fsRGBtoLinear(v.z), fsRGBtoLinear(v.w));
}

float4 linearToSRGB(const float4 v) {
	return (float4)(flinearToSRGB(v.x), flinearToSRGB(v.y), flinearToSRGB(v.z), flinearToSRGB(v.w));
}
