#include "config/ExecutableDirectory.h"
#include <windows.h>

std::string executableDirectory() {
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string path(buffer, length);
    return path.substr(0, path.find_last_of("\\/") + 1);
}
