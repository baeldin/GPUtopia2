#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"
//#include "GLFW\glfw3.h"

struct vec2
{
	float x;
	float y;
};

std::string loadShaderSourceFromFile(const std::string& filePath);
void deleteShaders(GLuint shaderProgram);
GLuint compileAndLinkShaders(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
void loadShaders(GLuint& shaderProgram, const bool reload = false);
void prepBuffers(GLuint& VAO, GLuint& VBO, GLuint& EBO, GLuint& shaderProgram, const int Nx, const int Ny);
