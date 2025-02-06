#include "selectFile.h"



void openFileDialog(std::string& fileName,
    bool& success, const std::wstring& defaultExt, 
    const std::wstring& filterDescription)
{
    success = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        OutputDebugStringW(L"CoInitializeEx failed.\n");
        return;
    }

    IFileOpenDialog* pFileOpen = nullptr;
    // Create the FileOpenDialog object.
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (FAILED(hr))
    {
        OutputDebugStringW(L"CoCreateInstance for FileOpenDialog failed.\n");
        CoUninitialize();
        return;
    }

    // Build a filter pattern from the default extension.
    // E.g. if defaultExt is L"txt", then filterPattern becomes L"*.txt"
    std::wstring filterPattern = L"*." + defaultExt;

    // Setup file type filters.
    COMDLG_FILTERSPEC rgSpec[] =
    {
        { filterDescription.c_str(), filterPattern.c_str() },
        { L"All Files (*.*)", L"*.*" }
    };

    hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
    if (FAILED(hr))
    {
        std::wstringstream ws;
        ws << L"SetFileTypes failed with hr = 0x" << std::hex << hr << L"\n";
        OutputDebugStringW(ws.str().c_str());
        pFileOpen->Release();
        CoUninitialize();
        return;
    }

    hr = pFileOpen->SetDefaultExtension(defaultExt.c_str());
    if (FAILED(hr))
    {
        std::wstringstream ws;
        ws << L"SetDefaultExtension failed with hr = 0x" << std::hex << hr << L"\n";
        OutputDebugStringW(ws.str().c_str());
        pFileOpen->Release();
        CoUninitialize();
        return;
    }

    // Show the Open dialog box.
    hr = pFileOpen->Show(NULL);
    if (FAILED(hr))
    {
        std::wstringstream ws;
        ws << L"FileOpenDialog::Show failed with hr = 0x" << std::hex << hr << L"\n";
        OutputDebugStringW(ws.str().c_str());
        pFileOpen->Release();
        CoUninitialize();
        return;
    }

    // Get the file name from the dialog box.
    IShellItem* pItem = nullptr;
    hr = pFileOpen->GetResult(&pItem);
    if (SUCCEEDED(hr))
    {
        PWSTR pszFilePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (SUCCEEDED(hr))
        {
            std::wstring tmpFilePath(pszFilePath);
            size_t sizeConverted = 0;
            fileName.resize(tmpFilePath.length());
            // Convert wide-character string to multibyte string.
            wcstombs_s(&sizeConverted, &fileName[0], fileName.size() + 1,
                tmpFilePath.c_str(), tmpFilePath.size());
            success = true;
        }
        else
        {
            OutputDebugStringW(L"GetDisplayName failed.\n");
        }
        CoTaskMemFree(pszFilePath);
        pItem->Release();
    }
    else
    {
        OutputDebugStringW(L"GetResult failed.\n");
    }

    pFileOpen->Release();
    CoUninitialize();
}

void saveFileDialog(std::string& fileName, bool& success)
{
    success = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileSaveDialog* pFileSave;

        // Create the FileSaveDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // Show the Save dialog box.
            hr = pFileSave->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        std::wstring tmpFilePath(pszFilePath);
                        size_t size;
                        fileName.resize(tmpFilePath.length());
                        wcstombs_s(&size, &fileName[0], fileName.size() + 1, tmpFilePath.c_str(), tmpFilePath.size());
                        success = true;
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
    // return 0;
}