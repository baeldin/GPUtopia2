#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_2_APIS

#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW\glfw3.h"
#include "cl_handler.h"
#include "gradient.h"
#include "display.h"
#include "fractal_navigation.h"

void ComputeStatistics(const std::vector<float> timings, float& average, float& median, float& stddev);
std::vector<float> CreateBinnedHistogram(const std::vector<float> timings, int numBins, float& outMin, float& outMax);

void infoWindow(clFractal& cf, fractalNavigationParameters& nav, ImFont* font_mono);