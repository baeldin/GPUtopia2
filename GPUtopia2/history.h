#pragma once

#include "cl_fractal.h"
#include "cl_core.h"

#include "json.hpp"
using json = nlohmann::ordered_json;

struct clLayerState;

void undo(clFractal& cf, clFractal& cf_old, clCore& cc, clLayerState& ls,
	std::vector<clFractalContainer>& history, int* historyIndex);

void redo(clFractal& cf, clFractal& cf_old, clCore& cc, clLayerState& ls,
	std::vector<clFractalContainer>& history, int* historyIndex);

void popHistory(std::vector<clFractalContainer>& history, int* historyIndex);