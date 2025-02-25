#pragma once

#include <CL/opencl.h>
#include <imgui.h>

#include "cl_core.h"
#include "cl_fractal.h"

class clCore;

void showErrorLogWindow(clFractal& cf, const clCore& cc, ImFont* font_mono, bool hasError);
