#pragma once
#include "stdafx.h"

// Key callback function
void key_callback_reload(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_S && mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
        std::cout << "Ctrl + S detected, reloading shaders..." << std::endl;
        GLuint* shaderProgramPtr = static_cast<GLuint*>(glfwGetWindowUserPointer(window));
        loadShaders(*shaderProgramPtr, true);
    }
}
