// constants and functions used for anti-aliasing and
// calculating subpixel coordinates of samples

real fracf(const real x) { return x - floor(x); }
real absf(const real x) { return x * sign(x); }
real2 absf2(const real2 x) { return x * sign(x); }

real __attribute__((overloadable)) tent(real x) {
    x = 2.f * x - 1.f;
    if (x == 0) { return 0.f; }
    return x / sqrt(absf(x)) - sign(x);
}

real2 __attribute__((overloadable)) tent(const real2 v) {
    return (real2)(tent(v.x), tent(v.y));
}

real half_tent(const real x) {
    return x == 0. ? 0. : 1. - x / sqrt(x);
}

real2 disc(const real2 v) {
    const real r = sqrt(half_tent(v.x));
    const real PI2 =  2. * 3.1415926535897932384626433;
    const real phi = PI2 * v.y;
    return (real2)(r * cos(phi), r * sin(phi));
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
