#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include <vector>
#include "glewGuard.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW\glfw3.h"

using std::string;

struct color;
void prepTexture(GLuint& texture);
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void drawBogusImg(std::vector<color>& img, int width, int height);

namespace mainView
{
	void mainViewPort();
};