#include "openFile.h"

void openFileDialog() {
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };
    HANDLE hReadPipe, hWritePipe;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create a pipe for standard output redirection
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        std::cerr << "Error creating pipe." << std::endl;
    }

    si.cb = sizeof(STARTUPINFO);
    si.hStdOutput = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Specify the executable path of the subprocess (in this example, "subprocess.exe")
    TCHAR szExePath[] = _T("./subprocess.exe");

    // Create the subprocess
    // if (CreateProcess(NULL, szExePath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cout << "Subprocess created successfully!" << std::endl;

        // Close the unused read pipe
        CloseHandle(hWritePipe);

        // Read and display output from the read pipe
        char buffer[4096];
        DWORD bytesRead; 
        while (ReadFile(hReadPipe, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            std::cout.write(buffer, bytesRead);
        }

        // Close the read pipe
        CloseHandle(hReadPipe);

        // Wait for the subprocess to finish
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    // }
    // else {
    //     std::cerr << "Error creating subprocess." << std::endl;
    // }
}