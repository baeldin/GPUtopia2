#pragma once

#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW\glfw3.h"
#include <vector>

#include "cl_fractal.h"
#include "gradient.h"
#include "display.h"
#include "clipboard.h"

void gradientWindow(clFractal& cf);