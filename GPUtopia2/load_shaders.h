#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL\glew.h"
#include "GLFW\glfw3.h"

std::string loadShaderSourceFromFile(const std::string& filePath)
{
    std::string shaderSource;
    std::ifstream shaderFile;

    // Ensure ifstream objects can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderSource = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR: Failed to read shader file " << filePath << std::endl;
    }
    return shaderSource;
}

void deleteShaders(GLuint shaderProgram) {
    glDeleteProgram(shaderProgram);
}

GLuint compileAndLinkShaders(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
    // Compile and link the shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void loadShaders(GLuint& shaderProgram, const bool reload = false) {
    if (reload) deleteShaders(shaderProgram);
    std::string vertexShaderSourceStr = loadShaderSourceFromFile("vertex_shader.glsl");
    std::string fragmentShaderSourceStr = loadShaderSourceFromFile("fragment_shader.glsl");
    const GLchar* vertexShaderSource = vertexShaderSourceStr.c_str();
    const GLchar* fragmentShaderSource = fragmentShaderSourceStr.c_str();
    shaderProgram = compileAndLinkShaders(vertexShaderSource, fragmentShaderSource);
}
