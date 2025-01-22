#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shobjidl.h> 
#include <string>
// #include <iostream>
// #include <locale>
// #include <codecvt>
#include <cstdlib>

void openFileDialog(std::string& fileName);