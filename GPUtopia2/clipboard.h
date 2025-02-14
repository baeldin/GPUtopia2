#pragma once
#include <windows.h>
#include <string>
#include <cstring>  // for memcpy

bool CopyStringToClipboard(const std::string& text);
std::string ReadStringFromClipboard();