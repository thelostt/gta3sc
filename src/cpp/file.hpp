#pragma once
#include <cstdio>
#include <string>
#include "optional.hpp"
#include "filesystem.hpp"

inline bool write_file(FILE* f, const void* data, size_t size)
{
    return (fwrite(data, 1, size, f) == size);
}

inline bool write_file(const fs::path& path, const void* data, size_t size)
{
#ifdef _WIN32
    auto f = _wfopen(path.c_str(), L"wb");
#else
    auto f = fopen(path.c_str(), "wb");
#endif
    
    if(f != nullptr)
    {
        bool result = write_file(f, data, size);
        fclose(f);
        return result;
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

inline auto read_file_binary(const fs::path& path) -> optional<std::vector<uint8_t>>
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
            std::vector<uint8_t> output;
            output.resize(size);

            if(fread(&output[0], sizeof(uint8_t), size, f) == size)
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

