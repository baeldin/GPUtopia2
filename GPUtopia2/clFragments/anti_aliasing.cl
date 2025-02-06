// constants and functions used for anti-aliasing and
// calculating subpixel coordinates of samples

__constant real phi = 1.618033988749f;
__constant real phi2 = 1.324717957244f;
__constant real inv_phi2A = 1.f / phi2;
__constant real inv_phi2B = 1.f / (phi2 * phi2);

real fracf(const real x) { return x - floor(x); }
real absf(const real x) { return x * sign(x); }
real2 absf2(const real2 x) { return x * sign(x); }

real tent(real x) {
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

// Hash function for 64 bit uint
// Found here: https://gist.github.com/degski/6e2069d6035ae04d5d6f64981c995ec2
ulong lowbias64(ulong x)
{
    x ^= x >> 32;
    x *= 0xd6e8feb86659fd93U;
    x ^= x >> 32;
    x *= 0xd6e8feb86659fd93U;
    x ^= x >> 32;
    return x;
}

//const float R2scale = 1.0f / (1 << 24);
//float2 __attribute__((overloadable)) R2_offset(const uint idx, const uint s)
//{
//    const uint R2offsetX = 3242174889u * s + lowbias32(2 * idx);
//    const uint R2offsetY = 2447445413u * s + lowbias32(2 * idx + 1);
//    return (float2)((R2offsetX >> 8) * R2scale, (R2offsetY >> 8) * R2scale);
//}

// 32 bit offset using 64 bit ints
//const double R2scale_fine = 1.0f / (1 << 24);
//float2  __attribute__((overloadable)) R2_offset(const uint idx, const uint s)
//{
//    const ulong R2offsetx = 13925035116211876495ul * (ulong)s + (ulong)lowbias64(2 * idx);
//    const ulong R2offsety = 10511698010929265437ul * (ulong)s + (ulong)lowbias64(2 * idx + 1);
//    return (float2)((R2offsetx >> 40) * R2scale_fine, (R2offsety >> 40) * R2scale_fine);
//}

// 32 bit offset using 64 bit ints
#ifdef USE_DOUBLE
__constant double R2scale = 1.0 / (1ul << 53);
__constant int bit_offset = 11;
#else
__constant float R2scale = 1.0f / (1 << 24);
__constant int bit_offset = 40;
#endif
real2 R2_offset(const uint idx, const uint s)
{
    const ulong R2offsetx = 13925035116211876495ul * (ulong)s + (ulong)lowbias64(2 * idx);
    const ulong R2offsety = 10511698010929265437ul * (ulong)s + (ulong)lowbias64(2 * idx + 1);
    return (real2)((R2offsetx >> bit_offset) * R2scale, (R2offsety >> bit_offset) * R2scale);
}
