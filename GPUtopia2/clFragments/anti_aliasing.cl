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

// Rational approximation of the inverse error function
// Based on the approximation by J.M. Blair (1976)
real erfinv(real x) {
    real w = -log((1.f - x) * (1.f + x));
    real p;
    if (w < 5.f) {
        w = w - 2.5f;
        p = 2.81022636e-08f;
        p = 3.43273939e-07f + p * w;
        p = -3.5233877e-06f + p * w;
        p = -4.39150654e-06f + p * w;
        p = 0.00021858087f + p * w;
        p = -0.00125372503f + p * w;
        p = -0.00417768164f + p * w;
        p = 0.246640727f + p * w;
        p = 1.50140941f + p * w;
    } else {
        w = sqrt(w) - 3.f;
        p = -0.000200214257f;
        p = 0.000100950558f + p * w;
        p = 0.00134934322f + p * w;
        p = -0.00367342844f + p * w;
        p = 0.00573950773f + p * w;
        p = -0.0076224613f + p * w;
        p = 0.00943887047f + p * w;
        p = 1.00167406f + p * w;
        p = 2.83297682f + p * w;
    }
    return p * x;
}

// Maps uniform [0,1] to a standard normal sample via inverse CDF
// Analogous to tent() but produces gaussian-distributed samples
real __attribute__((overloadable)) gaussian_tent(real x) {
    x = clamp(2.f * x - 1.f, (real)-0.9999f, (real)0.9999f);
    if (x == 0.f) { return 0.f; }
    return 1.41421356237f * erfinv(x); // sqrt(2) * erfinv(x)
}

real2 __attribute__((overloadable)) gaussian_tent(const real2 v) {
    return (real2)(gaussian_tent(v.x), gaussian_tent(v.y));
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
