// constants and functions used for anti-aliasing and
// calculating subpixel coordinates of samples

real fracf(const real x) { return x - floor(x); }
real absf(const real x) { return x * sign(x); }
real2 absf2(const real2 x) { return x * sign(x); }

real tent(real x) {
    x = 2.f * x - 1.f;
    if (x == 0) { return 0.f; }
    return x / sqrt(absf(x)) - sign(x);
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
