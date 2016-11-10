#pragma once
#include <stdinc.h>
#include "program.hpp"

class ConfigError : public std::runtime_error
{
public:
    template<typename... Args>
    ConfigError(const char* msg, Args&&... args)
        : std::runtime_error(fmt::format(msg, std::forward<Args>(args)...))
    {}
};
