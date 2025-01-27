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
#include "fractal_navigation.h"
#include "selectFile.h"
#include "cl_error_window.h"

using std::string;

struct color;
void prepTexture(GLuint& texture);
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);

namespace mainView
{
	void mainViewPort(ImFont* font_mono);
};