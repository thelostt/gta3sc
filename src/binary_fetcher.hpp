#pragma once
#include <cstdint>
#include <string>
#include <cpp/string_view.hpp>
#include <cpp/optional.hpp>

/// Interface to fetch little-endian bytes from a sequence of bytes in a easy and safe way.
struct BinaryFetcher
{
    const uint8_t* const bytes;
    const size_t         size;

    explicit BinaryFetcher(const void* bytes, size_t size) :
        bytes(reinterpret_cast<const uint8_t*>(bytes)), size(size)
    {}

    optional<uint8_t> fetch_u8(size_t offset) const noexcept
    {
        if(offset + 1 <= size)
        {
             return this->bytes[offset];
        }
        return nullopt;
    }

    optional<uint16_t> fetch_u16(size_t offset) const noexcept
    {
        if(offset + 2 <= size)
        {
            return uint16_t(this->bytes[offset+0]) << 0
                 | uint16_t(this->bytes[offset+1]) << 8;
        }
        return nullopt;
    }

    optional<uint32_t> fetch_u32(size_t offset) const noexcept
    {
        if(offset + 4 <= size)
        {
            return uint32_t(this->bytes[offset+0]) << 0
                 | uint32_t(this->bytes[offset+1]) << 8
                 | uint32_t(this->bytes[offset+2]) << 16
                 | uint32_t(this->bytes[offset+3]) << 24;
        }
        return nullopt;
    }

    optional<int8_t> fetch_i8(size_t offset) const noexcept
    {
        if(auto opt = fetch_u8(offset))
        {
            return reinterpret_cast<int8_t&>(*opt);
        }
        return nullopt;
    }

    optional<int16_t> fetch_i16(size_t offset) const noexcept
    {
        if(auto opt = fetch_u16(offset))
        {
            return reinterpret_cast<int16_t&>(*opt);
        }
        return nullopt;
    }

    optional<int32_t> fetch_i32(size_t offset) const noexcept
    {
        if(auto opt = fetch_u32(offset))
        {
            return reinterpret_cast<int32_t&>(*opt);
        }
        return nullopt;
    }

    optional<void*> fetch_bytes(size_t offset, size_t count, void* output) const noexcept
    {
        if(offset + count <= size)
        {
            std::memcpy(output, &this->bytes[offset], count);
            return output;
        }
        return nullopt;
    }

    optional<char*> fetch_chars(size_t offset, size_t count, char* output) const noexcept
    {
        if(offset + count <= size)
        {
            std::strncpy(output, reinterpret_cast<const char*>(&this->bytes[offset]), count);
            return output;
        }
        return nullopt;
    }

    optional<std::string> fetch_chars(size_t offset, size_t count) const noexcept
    {
        std::string str(count, '\0');
        if(count == 0 || fetch_chars(offset, count, &str[0]))
            return str;
        return nullopt;
    }

    optional<string_view> fetch_zstring(size_t offset) const noexcept
    {
        size_t curroff = offset;
        while(auto opt_byte = fetch_u8(curroff++))
        {
            if(*opt_byte == 0)
                return string_view(reinterpret_cast<const char*>(bytes+offset), curroff - offset - 1);
        }
        return nullopt;
    }
};
