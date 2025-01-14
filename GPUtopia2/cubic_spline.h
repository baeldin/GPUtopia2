#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <iostream>

#include "color.h"

using std::cout;

std::vector<double> solve_linear_eqs(std::vector<double> M, std::vector<double> b);
std::vector<double> calculate_spline_coefficients(std::vector<float> x, std::vector<float> y);
color splined_color(std::vector<float> x, std::vector<color> colors, float index);