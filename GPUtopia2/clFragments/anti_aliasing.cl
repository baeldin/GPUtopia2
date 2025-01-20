// constants and functions used for anti-aliasing and
// calculating subpixel coordinates of samples

float fracf(const float x) { return x - floor(x); }
float absf(const float x) { return x * sign(x); }
float2 absf2(const float2 x) { return x * sign(x); }

float tent(float x) {
    x = 2.f * x - 1.f;
    if (x == 0) { return 0.f; }
    return x / sqrt(absf(x)) - sign(x);
}

// Hash function for 32 bit uint
// Found here: https://nullprogram.com/blog/2018/07/31/
uint lowbias32(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

const uint uint_max = 0xffffffffu;
float tofloat(uint x) {
    return (float)x / (float)uint_max;
}

const float R2scale = 1.0f / (1 << 24);
float2 R2_offset(const uint idx, const uint s)
{
    const uint R2offsetX = 3242174889u * s + lowbias32(2 * idx);
    const uint R2offsetY = 2447445413u * s + lowbias32(2 * idx + 1);
    return (float2)((R2offsetX >> 8) * R2scale, (R2offsetY >> 8) * R2scale);
}

// unused 64 bit test
/*const double R2scale_fine = 1.0f / (1 << 24);
double2 R2_offset_fine(const uint idx, const uint s)
{
    const ulong R2offsetX = 13925035116211876495u * s + lowbias32(2 * idx);
    const ulong R2offsetY = 10511698010929265436u * s + lowbias32(2 * idx + 1);
    return (double2)((R2offsetX >> 8) * R2scale, (R2offsetY >> 8) * R2scale);
}*/
