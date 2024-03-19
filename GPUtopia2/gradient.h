#pragma once

#include <vector>
#include <random>
#include <CL/opencl.h>
#include <iostream>

#include "color.h"

// will bug out if x > 2.f or x < -1.f
// surely this will never cause problems
// right?
// RIGHT??
inline float wrap1(const float x) { 
    return (x > 1.f) ? x - 1 : (x < 0.f) ? x + 1 : x; }

inline float fract(const float x) {
    return x - floor(x);
}

struct colorNode
{
    int position;
    int index;
    color color;
};

float lerp(const float x, const float x0, const float x1,
    const float y0, const float y1);
color lerp(const float x, const float x0, const float x1,
    const color& c0, const color& c1);

class gradient
{
public:
    std::vector<colorNode> nodes;
    std::vector<color> fineColors;
    int nNodes = 4;
    int length = 400; // UF compat, can be changed later
    int fineLength = 20;
    // generate a random N-node gradient when this is used
    gradient();
    gradient(const int N);
    void fillFine();
    void printNodes();
    void printFine();
    color getColor(const float idx);
    std::vector<color> getGradientImg(const int width, const int height);
};