#pragma once
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <CL/opencl.h>

float clamp(const float x);

float sRGBtoLinear(const float x);
float linearToSRGB(const float x);

constexpr float inv255 = 1.f / 255.f;

struct color
{
    float r;
    float g;
    float b;
    float a = 1.f;
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
    color(const uint32_t UFint) {
        const int ib = UFint / 65536;
        const int ig = (UFint - b * 65536) / 256;
        const int ir = (UFint - b * 65536 - g * 256) / 256;
        r = (float)ir * inv255;
        g = (float)ig * inv255;
        b = (float)ib * inv255;
    }
    color operator+ (const color& c) {
        return color(r + c.r, g + c.g, b + c.b, a + c.a); }
    color operator* (const float& x) { 
        return color(r * x, g * x, b * x, a * x); }
    color operator/ (const float& x) { 
        const float inv_x = 1.f / x;
        return color(r * inv_x, g * inv_x, b * inv_x, a * inv_x); }
    color& operator= (const color& c)
    {
        if (this != &c) {
            r = c.r; g = c.g; b = c.b; a = c.a;
        }
        return *this;
    }
    cl_float4 to_cl_float4() { return cl_float4({ r, g, b, a }); }
    uint32_t toUFint() const {
        return 65536 * (int)(b * 255) + 256 * (int)(g * 255) + (int)(r * 255);
    }
};

inline const bool operator==(const color& lhs, const color& rhs)
{
    return(lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a);
}


std::ostream& operator<<(std::ostream& os, const color& c);

color clamp(const color& c);
color sRGBtoLinear(const color& c);
color linearToSRGB(const color& c);

