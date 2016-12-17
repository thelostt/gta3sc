#pragma once
#include <cstdio>
#include <string>
#include "optional.hpp"
#include "filesystem.hpp"
#include "../system.hpp"

inline FILE* u8fopen(const fs::path& path, const char* mode)
{
#ifdef _WIN32
    size_t i;
    wchar_t mode_wc[8];
    for(i = 0; mode[i] && i < 7; ++i)
        mode_wc[i] = mode[i];
    mode_wc[i] = L'\0';
    return _wfopen(path.c_str(), mode_wc);
#else
    return fopen(path.c_str(), mode);
#endif
}

/// \note see notes on `allocate_file`.
inline bool allocate_file_space(FILE* f, size_t size)
{
    return allocate_file(f, size);
}

inline size_t file_tell(FILE* f)
{
    return ftell(f);
}

inline bool write_file(FILE* f, const void* data, size_t size)
{
    return (fwrite(data, 1, size, f) == size);
}

inline bool write_file(FILE* f, size_t offset, const void* data, size_t size)
{
    if(fseek(f, offset, SEEK_SET) != 0)
        return false;
    return (fwrite(data, 1, size, f) == size);
}

inline bool write_file(const fs::path& path, const void* data, size_t size)
{
    if(auto f = u8fopen(path, "wb"))
    {
        bool result = write_file(f, data, size);
        fclose(f);
        return result;
    }
    return false;
}

inline auto read_file_utf8(const fs::path& path) -> optional<std::string>
{
    if(auto f = u8fopen(path, "rb"))
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
    if(auto f = u8fopen(path, "rb"))
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

//
// emulating file operations on std::vector<uint8_t>
//

inline bool allocate_file_space(std::vector<uint8_t>& vec, size_t size)
{
    try
    {
        if(vec.size() < size)
            vec.resize(size);
        return true;
    }
    catch(const std::bad_alloc&)
    {
        return false;
    }
}

inline size_t file_tell(std::vector<uint8_t>& vec)
{
    return vec.size();
}

inline bool write_file(std::vector<uint8_t>& vec, size_t offset, const void* data, size_t size)
{
    if(vec.size() < offset + size)
        vec.resize(offset + size);
    std::memcpy(vec.data() + offset, data, size);
    return true;
}
