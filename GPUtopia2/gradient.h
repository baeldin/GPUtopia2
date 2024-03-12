#pragma once

#include <vector>
#include <random>
#include <CL/opencl.h>

#include "color.h"

struct colorNode
{
    int position;
    int index;
    color color;
};

class gradient
{
    std::vector<colorNode> nodes;
    std::vector<colorNode> fineColors;
    int nNodes = 4;
    int length = 400; // UF compat, can be changed
    // generate a random N-node gradient when this is used
    gradient();
    gradient(const int N);
    
};