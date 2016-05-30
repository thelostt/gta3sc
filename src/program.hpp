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

template<typename... Args>
inline std::string format_error(const char* type, optional<const SyntaxTree&> context, const char* msg, Args&&... args)
{
    std::string prefix;
    std::string suffix;
    if(context)
    {
        // Let's do a bit of magic on context first.
        ///
        // If the supplied context has no text data associated with it, it's not very useful. It's likely that
        // its column is at the start of the line. So, find the first child (not too deep) that contains text data.
        if(!context->has_text())
        {
            for(auto it = context->begin(); it != context->end(); ++it)
            {
                if((*it)->has_text())
                {
                    context.emplace(**it);
                    break;
                }
            }
        }
        
        if(type)
            prefix = fmt::format("{}:{}:{}: {}: ", context->filename(), context->line(), context->column(), type);
        else
            prefix = fmt::format("{}:{}:{}: ", context->filename(), context->line(), context->column());

        if(auto tstream_ptr = context->token_stream().lock())
        {
            suffix = fmt::format("\n {}\n {:>{}}", tstream_ptr->get_line(context->line()), "^", context->column());
        }
    }
    else
    {
        if(type)
            prefix = fmt::format("{}: ", type);
        else
            /* prefix is empty */;
    }
    return fmt::format("{}{}{}", prefix, fmt::format(msg, std::forward<Args>(args)...), suffix);
}

class ProgramError
{
public:
    template<typename... Args>
    ProgramError(const SyntaxTree& context, const char* msg, Args&&... args)
        : message_(format_error("error", context, msg, std::forward<Args>(args)...))
    {}

    template<typename... Args>
    ProgramError(tag_nocontext_t, const char* msg, Args&&... args)
        : message_(format_error("error", nullopt, msg, std::forward<Args>(args)...))
    {}

    ProgramError(const SyntaxTree& context, const ProgramError& nocontext_error)
        : message_(format_error(nullptr, context, nocontext_error.message().c_str()))
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
        this->puts(pg_error.message().c_str());

        /* TODO limit error_count
        if(error_count > 100)
            fatal_error(nocontext, "XXX too many errors");
        */
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
        this->puts(format_error("warning", context, msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void fatal_error [[noreturn]] (const SyntaxTree& context, const char* msg, Args&&... args)
    {
        ++fatal_count;
        this->puts(format_error("fatal error", context, msg, std::forward<Args>(args)...));
        throw HaltJobException();
    }

    template<typename... Args>
    void fatal_error [[noreturn]](tag_nocontext_t, const char* msg, Args&&... args)
    {
        ++fatal_count;
        this->puts(format_error("fatal error", nullopt, msg, std::forward<Args>(args)...));
        throw HaltJobException();
    }

private:
    static void puts(const std::string& msg)
    {
        std::fprintf(stderr, "%s\n", msg.c_str());
    }

private:
    uint32_t error_count = 0;
    uint32_t fatal_count = 0;
    uint32_t warn_count  = 0;
};
