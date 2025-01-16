#pragma once

#include <vector>
#include <CL/opencl.h>
#include <imgui.h>

#include "complex_number.h"
#include "color.h"
#include "cl_fractal.h"


// struct to keep track of temporary variables used for mouse drag navigation
struct fractalNavigationParameters
{
	// general
	cl_int2 dragStart = { 0, 0 };
	cl_int2 dragOffset = { 0, 0 };
	ImVec2 coursorPos = { 0, 0 };
	ImVec2 coursorPosIo = { 0, 0 };
	// panning
	bool draggingCenter = false;
	Complex<float> centerOffset = { 0.f, 0.f };
	// zooming
	cl_int2 imgDisplayCenter = { 0, 0 };
	bool draggingZoom = false;
	float dragZoomFactor = 1.f;
	// rotating 
	bool draggingRotate = false;
};

void dragPan(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io);
void dragZoom(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io);

void offsetImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, 
	clFractal& cf, fractalNavigationParameters& nav);
void zoomImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage,
	clFractal& cf, fractalNavigationParameters& nav);

cl_float2 get_image_center_after_zoom(clFractal& cf, fractalNavigationParameters& nav);

Complex<float> get_complex_offset(const int dx, const int dy, const clFractal& cf);
// Complex<float> get_complex_coord(const cl_float2 xy_float, clFractal& cf);