#pragma once

#define _CRT_SECURE_NO_WARNINGS 

#include "cl_fractal.h"
#include "cl_core.h"


// forward declaration needed to avoid syntax error!?
class clCore;
struct clLayerState;

std::string getFileFromPath(std::string& fullPath);

void newCLF(clFractal& cf, clCore& cc, clLayerState& ls);
void openCLF(clFractal& cf, clCore& cc, clLayerState& ls);
void saveCLF(clFractal& cf);
void savePNG(const std::vector<color>& image_data, const clSize& size);
