#pragma once

#include "cl_fractal.h"
#include "cl_core.h"

#include "json.hpp"
using json = nlohmann::ordered_json;

class clCore;

void undo(clFractal& cf, clFractal& cf_old, clCore& cc, 
	std::vector<json>& history, int* historyIndex);

void redo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<json>& history, int* historyIndex);

void popHistory(std::vector<json>& history, int* historyIndex);