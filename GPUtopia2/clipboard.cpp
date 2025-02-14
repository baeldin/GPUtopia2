#include "clipboard.h"

bool CopyStringToClipboard(const std::string& text)
{
    // Open the clipboard; pass NULL to associate it with the current task.
    if (!OpenClipboard(nullptr))
        return false;

    // Empty the clipboard so we can set new data.
    if (!EmptyClipboard())
    {
        CloseClipboard();
        return false;
    }

    // Allocate a global memory object for the text.
    // +1 for the null terminator.
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hGlob)
    {
        CloseClipboard();
        return false;
    }

    // Lock the handle and copy the text to the buffer.
    char* pBuffer = static_cast<char*>(GlobalLock(hGlob));
    if (!pBuffer)
    {
        GlobalFree(hGlob);
        CloseClipboard();
        return false;
    }

    memcpy(pBuffer, text.c_str(), text.size() + 1);
    GlobalUnlock(hGlob);

    // Place the handle on the clipboard.
    // Note: CF_TEXT indicates ANSI text. For Unicode, use CF_UNICODETEXT and std::wstring.
    if (SetClipboardData(CF_TEXT, hGlob) == nullptr)
    {
        GlobalFree(hGlob);
        CloseClipboard();
        return false;
    }

    // The clipboard now owns the memory, so don't free it.
    CloseClipboard();
    return true;
}

std::string ReadStringFromClipboard() {
    std::string result;

    // Try to open the clipboard for the current process.
    if (!OpenClipboard(nullptr))
        return result;

    // Get the clipboard data in CF_TEXT (ANSI) format.
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData != nullptr) {
        // Lock the handle to get a pointer to the data.
        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (pszText != nullptr) {
            result = pszText; // Copy the clipboard data into our string.
            GlobalUnlock(hData);
        }
    }

    // Close the clipboard after finishing.
    CloseClipboard();
    return result;
}