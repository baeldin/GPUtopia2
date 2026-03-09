#include "clipboard.h"
#include "imgui.h"

bool CopyStringToClipboard(const std::string& text)
{
    ImGui::SetClipboardText(text.c_str());
    return true;
}

std::string ReadStringFromClipboard()
{
    const char* text = ImGui::GetClipboardText();
    return text ? std::string(text) : std::string();
}
