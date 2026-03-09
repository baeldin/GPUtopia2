#include "selectFile.h"
#include <tinyfiledialogs.h>

void openFileDialog(std::string& fileName,
    bool& success,
    const std::string& defaultExt,
    const std::string& filterDescription)
{
    success = false;
    std::string pattern = "*." + defaultExt;
    const char* filterPatterns[] = { pattern.c_str() };
    const char* result = tinyfd_openFileDialog(
        "Open File",
        "",
        1,
        filterPatterns,
        filterDescription.c_str(),
        0   // single select
    );
    if (result) {
        fileName = result;
        success = true;
    }
}

void saveFileDialog(std::string& fileName,
    bool& success,
    const std::string& defaultExt,
    const std::string& filterDescription)
{
    success = false;
    std::string pattern = "*." + defaultExt;
    const char* filterPatterns[] = { pattern.c_str() };
    const char* result = tinyfd_saveFileDialog(
        "Save File",
        ("untitled." + defaultExt).c_str(),
        1,
        filterPatterns,
        filterDescription.c_str()
    );
    if (result) {
        fileName = result;
        success = true;
    }
}
