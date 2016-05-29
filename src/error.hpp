#pragma once
#include "stdinc.h"
#include "program.hpp"

class ConfigError : public std::runtime_error
{
public:
    template<typename... Args>
    ConfigError(const char* msg, Args&&... args)
        : std::runtime_error(fmt::format(msg, std::forward<Args>(args)...))
    {}
};

class BadAlternator : public std::exception
{
public:
    template<typename... Args>
    BadAlternator(tag_nocontext_t, const char* msg, Args&&... args)
        : pg_error(tag_nocontext_t(), msg, std::forward<Args>(args)...)
    {}

    template<typename... Args>
    BadAlternator(const SyntaxTree& context, const char* msg, Args&&... args)
        : pg_error(context, msg, std::forward<Args>(args)...)
    {}

    template<typename... Args>
    BadAlternator(const SyntaxTree& context, const BadAlternator& nocontext_error)
        : pg_error(context, nocontext_error.pg_error)
    {}


    const char* what() const override
    {
        return this->pg_error.message().c_str();
    }

    const ProgramError& error() const
    {
        return this->pg_error;
    }

private:
    ProgramError pg_error;
};
