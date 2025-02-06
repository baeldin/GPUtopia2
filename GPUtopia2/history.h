#pragma once

#include "cl_fractal.h"
#include "cl_core.h"

class clCore;


void undo(clFractal& cf, clFractal& cf_old, clCore& cc, 
	std::vector<clFractalMinimal>& history, int* historyIndex);

void redo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<clFractalMinimal>& history, int* historyIndex);

void popHistory(std::vector<clFractalMinimal>& history, int* historyIndex);