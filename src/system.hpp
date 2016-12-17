///
/// Platform-Specific Details.
///
#pragma once
#include "cpp/filesystem.hpp"

/// Returns the path that static configuration is in.
extern const fs::path& config_path();

/// Allocates size for a file.
/// \warning the behaviour is undefined if the file isn't empty.
/// \note the file offset after this call is at the top of the file.
extern bool allocate_file(FILE*, uint64_t);
