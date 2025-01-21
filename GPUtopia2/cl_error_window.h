#pragma once

#include <CL/opencl.h>
#include <imgui.h>

#include "cl_core.h"
#include "cl_fractal.h"

class clCore;

void show_cl_error_window(clFractal& cf, const clCore& cc, ImFont* font_mono);
