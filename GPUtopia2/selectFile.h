#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shobjidl.h> 
#include <string>
// #include <iostream>
// #include <locale>
// #include <codecvt>
#include <cstdlib>
#include <comdef.h>
#include <sstream>

void openFileDialog(std::string& fileName,
    bool& success,
    const std::wstring& defaultExt = L"clf",
    const std::wstring& filterDescription = L"GPUtopia Fractal (*.clf)");
    
void saveFileDialog(std::string& fileName,
    bool& success, const std::wstring& defaultExt = L"clf",
    const std::wstring& filterDescription = L"GPUtopia Fractal (*.clf)");

