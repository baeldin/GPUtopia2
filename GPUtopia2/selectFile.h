#pragma once

#include <string>

void openFileDialog(std::string& fileName,
    bool& success,
    const std::string& defaultExt = "clf",
    const std::string& filterDescription = "GPUtopia Fractal (*.clf)");

void saveFileDialog(std::string& fileName,
    bool& success,
    const std::string& defaultExt = "clf",
    const std::string& filterDescription = "GPUtopia Fractal (*.clf)");
