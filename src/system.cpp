#include "system.hpp"

#ifdef _WIN32
#include <windows.h>
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
    throw std::runtime_error("find_config_path failed");
#elif defined(__unix__)
    const char* home_path = std::getenv("HOME");
    std::vector<fs::path> search_path;
    // Search path for unix, ordered by priority:
    {
        // In folder of the binary (temporary installations etc.)
        search_path.emplace_back("./config");
        // Home folder
        if(home_path != NULL)
        {
            search_path.emplace_back(fs::path(home_path) / ".local/share/gta3sc/config");
        }
        // Generic installation
        search_path.emplace_back("/usr/share/gta3sc/config");
    }

    for(auto& path : search_path)
    {
        if(fs::is_directory(path))
        {
            return path;
        }
    }
    throw std::runtime_error("find_config_path failed");
#else
	// TODO
	return "../../config";
#endif
}

fs::path config_path()
{
    static fs::path conf_path = find_config_path();
    return conf_path;
}
