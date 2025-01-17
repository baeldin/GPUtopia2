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
	ImVec2 MousePos = { 0, 0 };
	ImVec2 MousePosInImage = { 0, 0 };
	Complex<float> complexMousePos = Complex<float>(0.f, 0.f);
	// panning
	bool draggingCenter = false;
	Complex<float> centerOffset = { 0.f, 0.f };
	// zooming
	cl_float2 dragCenterDistance = { 0.f, 0.f };
	cl_int2 imgDisplayCenter = { 0, 0 };
	cl_float2 newImgCenter = { 0.f, 0.f };
	Complex<float> newSubplaneCenter = Complex<float>(0.f, 0.f);
	bool draggingZoom = false;
	float dragZoomFactor = 1.f;
	// rotating 
	bool draggingRotate = false;
	float dragAngle = 0.f;
	Complex<float> dragRotation = Complex<float>(0.f, 0.f);
};

void dragPan(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io);
void dragZoom(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io);
void dragRotate(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io);

void offsetImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, 
	clFractal& cf, fractalNavigationParameters& nav);
void zoomImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage,
	clFractal& cf, fractalNavigationParameters& nav);
void rotateImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage,
	clFractal& cf, fractalNavigationParameters& nav);
cl_float2 imgCoordinateCenterAfterZoom(clFractal& cf, fractalNavigationParameters& nav);

Complex<float> get_complex_offset(const int dx, const int dy, const clFractal& cf);