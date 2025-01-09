
const float phi = 1.618033988749f;
const float phi2 = 1.324717957244f;
const float inv_phi2A = 1.f / phi2;
const float inv_phi2B = 1.f / (phi2 * phi2);

float fracf(const float x) { return x - floor(x); }
float absf(const float x) { return x * sign(x); }
float2 absf2(const float2 x) { return x * sign(x); }

float2 fib_lattice(const int k, const int N) { 
    return (float2)(
        (float)k * phi, 
        (float)k / (float)N); }

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
