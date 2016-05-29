// TODO find a better name for this file / class!?
// program does not mean global as in 'for the whole program', it's just a context to be passed around.

// NOTE: not thread-safe context?

#pragma once
#include "stdinc.h"
#include "parser.hpp"

struct tag_nocontext_t {};
constexpr tag_nocontext_t nocontext;

// TODO better name?
struct HaltJobException : public std::exception
{
};

struct GameConfig
{
    // TODO maybe use bitfields?

    /// Boolean flags
    bool use_half_float = false;
    bool has_text_label_prefix = false;
};

class ProgramError
{
public:
    template<typename... Args>
    ProgramError(const SyntaxTree& context, const char* msg, Args&&... args)
        : message_(fmt::format("{}:{}:{}: error: {}\n", context.filename(), context.line(), context.column(),
                                                     fmt::format(msg, std::forward<Args>(args)...)))
    {}

    template<typename... Args>
    ProgramError(tag_nocontext_t, const char* msg, Args&&... args)
        : message_(fmt::format("error: {}\n", fmt::format(msg, std::forward<Args>(args)...)))
    {}

    ProgramError(const SyntaxTree& context, const ProgramError& nocontext_error)
        : message_(fmt::format("{}:{}:{}: {}\n", context.filename(), context.line(), context.column(), nocontext_error.message()))
    {}

    const std::string& message() const
    {
        return this->message_;
    }

protected:
    std::string message_;
};

class ProgramContext
{
public:
    const GameConfig opt;   ///< Compiler options / flags.

public:
    explicit ProgramContext(GameConfig opt) :
        opt(std::move(opt))
    {}

    ProgramContext(const ProgramContext&) = delete;
    ProgramContext(ProgramContext&&) = delete;

    bool has_error() const
    {
        return this->error_count > 0;
    }

    void error(const ProgramError& pg_error)
    {
        ++error_count;
        this->print(pg_error.message().c_str());
    }

    template<typename... Args>
    void error(const SyntaxTree& context, const char* msg, Args&&... args)
    {
        return error(ProgramError(context, msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void error(tag_nocontext_t, const char* msg, Args&&... args)
    {
        return error(ProgramError(tag_nocontext_t(), msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void warning(const SyntaxTree& context, const char* msg, Args&&... args)
    {
        ++warn_count;
        this->print4(context, "{}:{}:{}: warning: {}\n", fmt::format(msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void fatal_error [[noreturn]] (const SyntaxTree& context, const char* msg, Args&&... args)
    {
        ++fatal_count;
        this->print4(context, "{}:{}:{}: fatal error: {}\n", fmt::format(msg, std::forward<Args>(args)...));
        throw HaltJobException();
    }

    template<typename... Args>
    void fatal_error [[noreturn]](tag_nocontext_t, const char* msg, Args&&... args)
    {
        ++fatal_count;
        this->print("fatal error: {}\n", fmt::format(msg, std::forward<Args>(args)...));
        throw HaltJobException();
    }

private:
    template<typename... Args>
    static void print(const char* msg, Args&&... args)
    {
        // TODO lock stderr for thread-safe print? maybe standard already mandates that?
        fmt::fprintf(stderr, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void print4(const SyntaxTree& context, const char* msg, Args&&... args)
    {
        // TODO lock stderr for thread-safe print? maybe standard already mandates that?
        fmt::fprintf(stderr, msg, context.filename(), context.line(), context.column(), std::forward<Args>(args)...);
    }

private:
    uint32_t error_count = 0;
    uint32_t fatal_count = 0;
    uint32_t warn_count  = 0;
};
