#pragma once
#include <cstdio>
#include <string>
#include "optional.hpp"
#include "filesystem.hpp"

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

// may be sparse, the FILE pointer after this call is undefined
inline bool allocate_file_space(FILE* f, size_t size)
{
    // TODO check return status of fseek
    // TODO, fseek past eof isn't well-defined by the standard, do it in another way
    if(size > 0)
    {
        fseek(f, size - 1, SEEK_SET);
        fputc(0, f);
    }
    return true;
}

inline bool allocate_file_space(std::vector<uint8_t>& vec, size_t size)
{
    if(vec.size() < size)
        vec.resize(size);
    return true;
}

inline bool write_file(std::vector<uint8_t>& vec, size_t offset, const void* data, size_t size)
{
    allocate_file_space(vec, offset + size);
    std::memcpy(vec.data() + offset, data, size);
    return true;
}

inline bool write_file(std::vector<uint8_t>& vec, const void* data, size_t size)
{
    return write_file(vec, vec.size(), data, size);
}
