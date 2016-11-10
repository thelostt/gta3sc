#include <stdinc.h>
#include "system.hpp"
#include <climits>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <unistd.h>
#endif

static fs::path find_config_path()
{
#if defined(_WIN32)
    wchar_t szPathBuffer[MAX_PATH];
    DWORD dwResult = GetModuleFileNameW(NULL, szPathBuffer, std::size(szPathBuffer));
    if(dwResult != 0 && dwResult < std::size(szPathBuffer))
    {
        fs::path path(szPathBuffer);
        path.replace_filename(L"config");
        return path;
    }
    throw std::runtime_error("find_config_path failed");
#elif defined(__unix__)
    const char* home_path = std::getenv("HOME");
    std::vector<fs::path> search_path;

    // Search path for unix, ordered by priority:
    {
        // In folder of the binary (temporary installations etc.)
        #if defined(__linux__) || defined(__FreeBSD__)
        {
            char path_buffer[PATH_MAX];
            #if defined(__linux__)
            const char* exe_link_path = "/proc/self/exe";
            #elif defined(__FreeBSD__)
            const char* exe_link_path = "/proc/curproc/file";
            #else
            #error
            #endif

            ssize_t outlen = readlink(exe_link_path, path_buffer, std::size(path_buffer) - 1);
            if(outlen != -1)
            {
                fs::path path(path_buffer, path_buffer + outlen);
                path.replace_filename("config");
                search_path.emplace_back(std::move(path));
            }
        }
        #endif

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
            return path;
    }

    throw std::runtime_error("find_config_path failed");
#else
#   error find_config_path not implemented for this platform.
#endif
}

const fs::path& config_path()
{
    static fs::path conf_path = find_config_path();
    return conf_path;
}
