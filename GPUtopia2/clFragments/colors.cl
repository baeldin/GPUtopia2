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

float lerp(const float x, const float x0, const float x1,
	const float y0, const float y1)
{
	return ((x - x0) * y1 + (x1 - x) * y0) / (x1 - x0);
}

float4 f4lerp(const float x, const float x0, const float x1,
	const float4 y0, const float4 y1)
{
	return (float4)(
		lerp(x, x0, x1, y0.x, y1.x), lerp(x, x0, x1, y0.y, y1.y),
		lerp(x, x0, x1, y0.z, y1.z), lerp(x, x0, x1, y0.w, y1.w));
}

// turn a float4 color into an 8 bit int4 color which is suitable for
// use with atomic_add
int4 toIntColor(const float4 color)
{
	return (int4)(
		(int)(256 * color.x),
		(int)(256 * color.y),
		(int)(256 * color.z),
		(int)(256 * color.w));
}

// get a color from a gradient via linear interpolation
// TODO: figure out why the function fails at the rollover point
int4 getColor(const float4* colors, const float idxIn, const int nColors)
{
	const float fidx = idxIn - floor(idxIn);
	const int colorIndex1 = floor(fidx * nColors);
	const int colorIndex2 = (colorIndex1 == nColors - 1) ? 0 : colorIndex1 + 1;
	float4 outColor = sRGBtoLinear(
		f4lerp(
			fidx, ceil(fidx), floor(fidx), 
			colors[colorIndex1], colors[colorIndex2]));
	return toIntColor(outColor);
}
