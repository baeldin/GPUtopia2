#pragma once

#include <CL/opencl.h>
#include <imgui.h>

#include "cl_core.h"
#include "cl_fractal.h"

struct clLayerState;

void showErrorLogWindow(clFractal& cf, const clLayerState& ls, ImFont* font_mono, bool hasError);
