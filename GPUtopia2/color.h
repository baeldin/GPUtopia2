#pragma once

#include <cmath>
#include <CL/opencl.h>

float clamp(const float x);

float sRGBtoLinear(const float x);
float linearToSRGB(const float x);

struct color
{
    float r;
    float g;
    float b;
    float a = 1.;
    color() : r(0.f), g(0.f), b(0.f), a(1.f) {}
    color(const float x) : r(x), g(x), b(x), a(1.f) {}
    color(const float c, const float a_) : r(c), g(c), b(c), a(a_) {}
    color(const float r_, const float g_, const float b_)
    {
        r = r_, g = g_, b = b_, a = 1.f;
    }
    color(const float r_, const float g_, const float b_, const float a_)
    {
        r = r_; g = g_; b = b_; a = a_;
    }
    color operator+ (const color& c) { r += c.r; g += c.g; b += c.b; }
    color operator* (const float& x) { r *= x, g *= x; b *= x; }
    color operator/ (const float& x) { r /= x; g /= x; b /= x; }
    cl_float4 to_cl_float4() { return cl_float4({ r, g, b, a }); }

};

color clamp(const color& c);
color sRGBtoLinear(const color& c);
color linearToSRGB(const color& c);

