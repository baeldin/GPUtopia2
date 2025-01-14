#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <thread>

#include <vector>
#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW\glfw3.h"
#include "load_shaders.h"
#include "cl_core.h"
#include "formulaSettings.h"
#include "imageSettings.h"
#include "flameRenderSettings.h"
#include "cl_handler.h"
#include "explorer.h"
#include "info.h"
#include "test_gradients.h"
#include "sampling.h"
#include "complex_number.h"

using std::string;

struct color;
void prepTexture(GLuint& texture);
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void drawBogusImg(std::vector<color>& img, int width, int height);

void zoomImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, const float dragZoomFactor,
	const int centerX, const int centerY, const int imgWidth, const int imgHeight);

void offsetImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, 
	const int dx, const int dy, const int imgWidth, const int imgHeight);


Complex<float> get_complex_offset(const float dx, const float dy, const clFractal& cf);
Complex<float> get_complex_coord(const float x_shifted, const float y_shifted, const clFractal& cf);

namespace mainView
{
	void mainViewPort();
};