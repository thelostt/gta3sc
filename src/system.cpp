#include "system.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#endif

static fs::path find_config_path()
{
#ifdef _WIN32
    wchar_t szPathBuffer[MAX_PATH];
    DWORD dwResult = GetModuleFileNameW(NULL, szPathBuffer, std::size(szPathBuffer));
    if(dwResult != 0 && dwResult < std::size(szPathBuffer))
    {
        fs::path path(szPathBuffer);
        path.remove_filename();
        path /= L"../../.."; // TODO remove me, needed only when debug building
        path /= L"config";
        return path;
    }
    throw std::runtime_error("find_proc_path failed");
#else
#   error find_proc_path not implemented on this OS.
#endif
}

fs::path config_path()
{
    static fs::path conf_path = find_config_path();
    return conf_path;
}
