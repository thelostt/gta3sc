#pragma once
#include <cstdio>
#include <string>
#include "optional.hpp"
#include "filesystem.hpp"

inline bool write_file(const fs::path& path, const void* data, size_t size)
{
#ifdef _WIN32
    auto f = _wfopen(path.c_str(), L"wb");
#else
    auto f = fopen(path.c_str(), "wb");
#endif
    
    if(f != nullptr)
    {
        if(fwrite(data, 1, size, f) == size)
        {
            fclose(f);
            return true;
        }

        fclose(f);
    }

    return false;
}

inline auto read_file_utf8(const fs::path& path) -> optional<std::string>
{
#ifdef _WIN32
    auto f = _wfopen(path.c_str(), L"rb");
#else
    auto f = fopen(path.c_str(), "rb");
#endif

    if(f != nullptr)
    {
        size_t size;
        if(!fseek(f, 0L, SEEK_END) && (size = ftell(f)) != -1 && !fseek(f, 0L, SEEK_SET))
        {
            std::string output;
            output.resize(size);

            if(fread(&output[0], sizeof(char), size, f) == size)
            {
                fclose(f);
                return output;
            }
        }

        fclose(f);
        return nullopt;
    }

    return nullopt;
}

