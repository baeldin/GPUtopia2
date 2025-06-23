#pragma once

#define _CRT_SECURE_NO_WARNINGS 

#include "cl_fractal.h"
#include "cl_core.h"


// forward declaration needed to avoid syntax error!?
class clCore; 

std::string getFileFromPath(std::string& fullPath);

void newCLF(clFractal& cf, clCore& cc);
void openCLF(clFractal& cf, clCore& cc);
void saveCLF(clFractal& cf);
void savePNG(const std::vector<color>& image_data, const size& size);
