#pragma once
#include <cstdint>

struct alignas(4) CdHeader
{
    char magic[4];
    uint32_t num_entries;
};

struct alignas(4) CdEntry
{
    uint32_t offset;                // in sectors
    uint16_t streaming_size;        // in sectors
    uint16_t size_in_archive = 0;   // in sectors
    char     filename[24];
};
