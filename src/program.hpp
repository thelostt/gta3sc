// TODO find a better name for this file / class!?
// program does not mean global as in 'for the whole program', it's just a context to be passed around.

// NOTE: not thread-safe context?

#pragma once
#include "stdinc.h"
#include "parser.hpp"
#include "symtable.hpp"

struct tag_nocontext_t {};
constexpr tag_nocontext_t nocontext;

// TODO better name?
struct HaltJobException : public std::exception
{
};

struct Options
{
    // TODO maybe use bitfields?

    /// Boolean flags
    bool pedantic = false;
    bool use_half_float = false;
    bool has_text_label_prefix = false;
};

template<typename... Args>
inline std::string format_error(const char* type,
                                const shared_ptr<const TokenStream>& tstream_ptr,
                                const char* filename, uint32_t lineno, uint32_t colno,
                                const char* msg, Args&&... args)
{
    std::string message;
    message.reserve(255);

    if(filename)
    {
        message += filename;
        message.push_back(':');
    }

    if(lineno)
    {
        message += std::to_string(lineno);
        message.push_back(':');
    }

    if(colno)
    {
        message += std::to_string(colno);
        message.push_back(':');
    }

    if(message.size())
    {
        message.push_back(' ');
    }

    if(type)
    {
        message += type;
        message += ": ";
    }

    message += fmt::format(msg, std::forward<Args>(args)...);

    if(tstream_ptr)
    {
        message += fmt::format("\n {}\n {:>{}}", tstream_ptr->get_line(lineno), "^", colno);
    }

    return message;
}

template<typename... Args>
inline std::string format_error(const char* type, const SyntaxTree& base_context, const char* msg, Args&&... args)
{
    const SyntaxTree* context = &base_context;

    // Let's change the context a bit if necessary.
    ///
    // If the supplied context has no text data associated with it, it's not very useful. It's likely that
    // its column is at the start of the line. So, find the first child (not too deep) that contains text data.
    if(!context->has_text())
    {
        for(auto it = context->begin(); it != context->end(); ++it)
        {
            if((*it)->has_text())
            {
                context = &(**it);
                break;
            }
        }
    }

    return format_error(type, context->token_stream().lock(),
                              context->filename().c_str(), context->line(), context->column(),
                              msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline std::string format_error(const char* type, const Script& script, const char* msg, Args&&... args)
{
    return format_error(type, nullptr, script.path.generic_u8string().c_str(), 0, 0, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline std::string format_error(const char* type, tag_nocontext_t, const char* msg, Args&&... args)
{
    return format_error(type, nullptr, nullptr, 0, 0, msg, std::forward<Args>(args)...);
}



// TODO this is a weird type, we should probably get rid of it, leave only the program.error function
class ProgramError
{
public:
    template<typename... Args>
    ProgramError(const SyntaxTree& context, const char* msg, Args&&... args)
        : message_(format_error("error", context, msg, std::forward<Args>(args)...))
    {}

    template<typename... Args>
    ProgramError(const Script& context, const char* msg, Args&&... args)
        : message_(format_error("error", context, msg, std::forward<Args>(args)...))
    {}

    template<typename... Args>
    ProgramError(tag_nocontext_t, const char* msg, Args&&... args)
        : message_(format_error("error", nocontext, msg, std::forward<Args>(args)...))
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
    const Options opt;   ///< Compiler options / flags.

public:
    explicit ProgramContext(Options opt) :
        opt(std::move(opt))
    {}

    ProgramContext(const ProgramContext&) = delete;
    ProgramContext(ProgramContext&&) = delete;

    bool is_model_from_ide(const std::string& name) const
    {
        // TODO elaborate
        if(iequal_to()(name, "PLAYERSDOOR")
        || iequal_to()(name, "DEADMAN1")
        || iequal_to()(name, "BACKDOOR")
        || iequal_to()(name, "HELIX_BARRIER")
        || iequal_to()(name, "AIRPORTDOOR1")
        || iequal_to()(name, "AIRPORTDOOR2"))
            return true;
        return false;
    }

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
    void error(const Script& context, const char* msg, Args&&... args)
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
        this->puts(format_error("fatal error", nocontext, msg, std::forward<Args>(args)...));
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
