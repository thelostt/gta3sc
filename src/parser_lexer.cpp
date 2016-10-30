#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

using TokenData = TokenStream::TokenData;

struct LexerContext
{
    ProgramContext& program;
    TokenStream::TextStream stream;

    bool any_error = false;                 //< True if any error happened during tokenization.
    size_t comment_nest_level = 0;          //< Nest level of /* comments */
    std::vector<TokenData> tokens;          //< Output tokens.
    std::string            line_buffer;     //< Buffer used to parse a line, since we'll be mutating the line.

    explicit LexerContext(ProgramContext& program, std::string data, std::string stream_name) :
        program(program), stream(std::move(data), std::move(stream_name))
    {}

    void verify_nesting()
    {
        if(this->comment_nest_level != 0)
            this->error(std::make_pair(0, 0), "XXX end of file without closing */");
    }

    void add_token(Token type, size_t begin_pos, size_t length)
    {
        this->tokens.emplace_back(TokenData{ type, begin_pos, begin_pos + length });
    }

    template<typename... Args>
    void error(std::pair<size_t, size_t> pos, Args&&... args) // pos = <begin_pos, size>
    {
        this->any_error = true;
        this->program.error(TokenStream::TokenInfo(this->stream, pos.first, pos.first + pos.second),
                            std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(size_t pos, Args&&... args)
    {
        return error(std::make_pair(pos, pos+1), std::forward<Args>(args)...);
    }
};

#define DEFINE_TOKEN(kw)          { #kw, Token :: kw }
#define DEFINE_SYMBOL(sym, tok)   { sym, tok }

static const std::pair<string_view, Token> keycommands[] = {
    DEFINE_SYMBOL("{", Token::ScopeBegin),
    DEFINE_SYMBOL("}", Token::ScopeEnd),
    //DEFINE_TOKEN(NOT),
    //DEFINE_TOKEN(AND),
    //DEFINE_TOKEN(OR),
    //DEFINE_TOKEN(IF),
    DEFINE_TOKEN(ELSE),
    DEFINE_TOKEN(ENDIF),
    //DEFINE_TOKEN(WHILE),
    DEFINE_TOKEN(ENDWHILE),
    DEFINE_TOKEN(REPEAT),
    DEFINE_TOKEN(ENDREPEAT),
    DEFINE_TOKEN(SWITCH),
    DEFINE_TOKEN(ENDSWITCH),
    DEFINE_TOKEN(CASE),
    DEFINE_TOKEN(DEFAULT),
    DEFINE_TOKEN(BREAK),
    DEFINE_TOKEN(CONTINUE),
    DEFINE_TOKEN(MISSION_START),
    DEFINE_TOKEN(MISSION_END),
    DEFINE_TOKEN(SCRIPT_START),
    DEFINE_TOKEN(SCRIPT_END),
    DEFINE_TOKEN(VAR_INT),
    DEFINE_TOKEN(LVAR_INT),
    DEFINE_TOKEN(VAR_FLOAT),
    DEFINE_TOKEN(LVAR_FLOAT),
    DEFINE_TOKEN(VAR_TEXT_LABEL),
    DEFINE_TOKEN(LVAR_TEXT_LABEL),
    DEFINE_TOKEN(VAR_TEXT_LABEL16),
    DEFINE_TOKEN(LVAR_TEXT_LABEL16),
};

static const std::pair<string_view, size_t> script_registers[] = {
    // COMMAND                                  FILENAME_ID
    { "LAUNCH_MISSION",                                 1 },
    { "LOAD_AND_LAUNCH_MISSION",                        1 },
    { "GOSUB_FILE",                                     2 },
    { "REGISTER_STREAMED_SCRIPT",                       1 },
    { "REGISTER_SCRIPT_BRAIN_FOR_CODE_USE",             1 },
    { "REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE",   1 },
    { "STREAM_SCRIPT",                                  1 },
    { "HAS_STREAMED_SCRIPT_LOADED",                     1 },
    { "MARK_STREAMED_SCRIPT_AS_NO_LONGER_NEEDED",       1 },
    { "REMOVE_STREAMED_SCRIPT",                         1 },
    { "REGISTER_STREAMED_SCRIPT",                       1 },
    { "START_NEW_STREAMED_SCRIPT",                      1 },
    { "GET_NUMBER_OF_INSTANCES_OF_STREAMED_SCRIPT",     1 },
    { "ALLOCATE_STREAMED_SCRIPT_TO_RANDOM_PED",         1 },
    { "ALLOCATE_STREAMED_SCRIPT_TO_OBJECT",             1 },
    { "REGISTER_OBJECT_SCRIPT_BRAIN_FOR_CODE_USE",      1 },
    { "ALLOCATE_STREAMED_SCRIPT_TO_PED_GENERATOR",      1 },
};

static const std::pair<string_view, Token> expr_symbols[] = {
    // Order matters (by length)
    // 3-length
    DEFINE_SYMBOL("+=@", Token::EqTimedPlus),
    DEFINE_SYMBOL("-=@", Token::EqTimedMinus),
    // 2-length
    DEFINE_SYMBOL("+=", Token::EqPlus),
    DEFINE_SYMBOL("-=", Token::EqMinus),
    DEFINE_SYMBOL("*=", Token::EqTimes),
    DEFINE_SYMBOL("/=", Token::EqDivide),
    DEFINE_SYMBOL("++", Token::Increment),
    DEFINE_SYMBOL("--", Token::Decrement),
    DEFINE_SYMBOL("=#", Token::EqCast),
    DEFINE_SYMBOL("+@", Token::TimedPlus),
    DEFINE_SYMBOL("-@", Token::TimedMinus),
    DEFINE_SYMBOL("<=", Token::LesserEqual),
    DEFINE_SYMBOL(">=", Token::GreaterEqual),
    // 1-length
    DEFINE_SYMBOL("=", Token::Equal),
    DEFINE_SYMBOL("+", Token::Plus),
    DEFINE_SYMBOL("-", Token::Minus),
    DEFINE_SYMBOL("*", Token::Times),
    DEFINE_SYMBOL("/", Token::Divide),
    DEFINE_SYMBOL("<", Token::Lesser),
    DEFINE_SYMBOL(">", Token::Greater),
};

/// Checks if `c` is a whitespace.
static bool lex_iswhite(int c)
{
    return c == ' ' || c == '\t' || c == '(' || c == ')' || c == ',' || c == '\r';
}

/// Checks if `token` is equal `string` which has `length`.
static bool lex_istokeq(const std::pair<const char*, size_t>& token, const char* string, size_t length)
{
    return (token.second == length && !strncasecmp(token.first, string, length));
}

/// Checks if `token` is equal to the string literal `string`.
template<std::size_t N>
static bool lex_istokeq(const std::pair<const char*, size_t>& token, const char(&string)[N])
{
    static_assert(N > 0, "");
    return lex_istokeq(token, string, N - 1);
}

/// Checks if `token` is equal to the string in the view `string`.
static bool lex_istokeq(const std::pair<const char*, size_t>& token, const string_view& string)
{
    return lex_istokeq(token, string.data(), string.size());
}

/// Checks if `it` is part of a expression token.
static bool lex_isexprc(const char* it, const char* end)
{
    if(*it == '-')
    {
        auto next = std::next(it);
        if(next != end)
        {
            if(*next == '.' || (*next >= '0' && *next <= '9'))
                return false;
        }
        return true;
    }
    return (*it == '+' || *it == '*' || *it == '/' || *it == '=' || *it == '<' || *it == '>');
}

/// Gets the next whitespace delimited token in `it`.
static auto lex_gettok(const char* it, const char* end) -> optional<std::pair<const char*, size_t>>
{
    auto tok_start = std::find_if_not(it, end, lex_iswhite);
    auto tok_end   = std::find_if(tok_start, end, lex_iswhite);

    if(tok_start != tok_end)
        return std::make_pair(tok_start, size_t(tok_end - tok_start));

    return nullopt;
}

/// Reads the next token (number/identifier/string) from `begin` into the `lexer` state. 
static auto lex_token(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos) -> const char*
{
    while(begin != end && lex_iswhite(*begin))
        ++begin, ++begin_pos;

    if(begin == end)
        return end;

    auto is_integer = [](const std::pair<const char*, size_t>& token)
    {
        for(size_t i = 0; i < token.second; ++i)
        {
            if(token.first[i] < '0' || token.first[i] > '9')
            {
                if(token.first[i] == '-')
                {
                    if(i != 0)
                    {
                        // TODO emit warning
                    }
                    continue;
                }
                return false;
            }
        }
        return true;
    };

    auto is_float = [](const std::pair<const char*, size_t>& token)
    {
        for(size_t i = 0; i < token.second; ++i)
        {
            if(token.first[i] < '0' || token.first[i] > '9')
            {
                if(token.first[i] == '.' || (token.first[i] == '-' && i == 0))
                    continue;

                if(token.first[i] == 'f' || token.first[i] == 'F')
                {
                    if(i+1 != token.second)
                    {
                        // TODO emit warning
                    }
                    continue;
                }

                return false;
            }
        }
        return true;
    };

    if(*begin == '"')
    {
        auto it = std::find(std::next(begin), end, '"');
        if(it == end)
        {
            lexer.error(begin_pos, "XXX end of line without closing quotes");
            return end;
        }
        return std::next(it);
    }
    else if((*begin >= '0' && *begin <= '9') || *begin == '-' || *begin == '.')
    {
        auto token = lex_gettok(begin, end).value();

        if(is_integer(token))
            lexer.add_token(Token::Integer, begin_pos, token.second);
        else if(is_float(token))
            lexer.add_token(Token::Float, begin_pos, token.second);
        else
            lexer.error(std::make_pair(begin_pos, token.second), "XXX invalid numeric literal");

        return token.first + token.second;
    }
    else if((*begin >= 'a' && *begin <= 'z') || (*begin >= 'A' && *begin <= 'Z') || *begin == '$')
    {
        auto token = lex_gettok(begin, end).value();
        auto it    = token.first + token.second;
        lexer.add_token(Token::Identifier, begin_pos, std::distance(begin, it));
        return it;
    }
    else
    {
        auto token = lex_gettok(begin, end).value();
        lexer.error(std::make_pair(begin_pos, token.second), "XXX invalid identifier");
        return token.first + token.second;
    }
}

/// Lexes a command context.
static void lex_command(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos, bool had_keycommand)
{
    if(auto cmdtok = lex_gettok(begin, end))
    {
        bool is_keycommand = false;
        size_t script_register_id = SIZE_MAX;

        auto it = cmdtok->first + cmdtok->second;

        for(auto& keycmd : keycommands)
        {
            if(lex_istokeq(*cmdtok, keycmd.first))
            {
                size_t tok_begin = begin_pos + std::distance(begin, cmdtok->first);
                
                if(had_keycommand)
                    lexer.error(std::make_pair(tok_begin, cmdtok->second), "unexpected token");

                lexer.add_token(keycmd.second, tok_begin, cmdtok->second);
                is_keycommand = true;
                break;
            }
        }

        if(!is_keycommand)
        {
            size_t tok_begin = begin_pos + std::distance(begin, cmdtok->first);
            lexer.add_token(Token::Command, tok_begin, cmdtok->second);

            for(auto& regcmd : script_registers)
            {
                if(lex_istokeq(*cmdtok, regcmd.first))
                {
                    script_register_id = regcmd.second;
                    break;
                }
            }
        }

        for(size_t id = 1; it != end; ++id)
        {
            if(id != script_register_id)
            {
                it = lex_token(lexer, it, end, begin_pos + std::distance(begin, it));
            }
            else
            {
                // this is a filename token, just get whatever is delimited by spaces
                if(auto token = lex_gettok(it, end))
                {
                    lexer.add_token(Token::Identifier, begin_pos + std::distance(begin, token->first) , token->second);
                    it = token->first + token->second;
                }
            }
        }
    }
}

/// Lexes a expression context. 
static void lex_expr(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos)
{
    for(auto it = begin; ;)
    {
        it = std::find_if_not(it, end, lex_iswhite);

        if(it == end)
            break;

        bool found_symbol = false;
        auto length_left = size_t(std::distance(it, end));
        
        for(auto& symbol : expr_symbols)
        {
            if(length_left >= symbol.first.size()
            && lex_isexprc(it, end) // for special minus checking
            && !strncmp(it, symbol.first.data(), symbol.first.size()))
            {
                size_t pos = (begin_pos + std::distance(begin, it));
                lexer.add_token(symbol.second, pos, symbol.first.size());
                it = std::next(it, symbol.first.size());
                found_symbol = true;
                break;
            }
        }

        if(!found_symbol)
        {
            // Cannot use lex_gettok due to it not being only lex_iswhite delimited, but also lex_exprc delimited.
            auto tok_begin = it;
            while(it != end && !lex_iswhite(*it) && !lex_isexprc(it, end)) ++it;
            auto tok_end = it;

            it = lex_token(lexer, tok_begin, tok_end, begin_pos + std::distance(begin, tok_begin));
        }
    }
}

/// Tranforms comments into whitespaces.
static void lex_comments(LexerContext& lexer, char* begin, char* end, size_t begin_pos)
{
    bool in_quotes = false;

    for(auto it = begin; it != end; ++it)
    {
        if(*it == '"' && lexer.comment_nest_level == 0)
        {
            in_quotes = !in_quotes;
        }
        else if(std::next(it) != end)
        {
            if(in_quotes)
            {
                // Cannot have a comment inside a quoted string.
            }
            else if(*it == '/' && *std::next(it) == '/')
            {
                std::memset(it, ' ', end - it);
                return;
            }
            else if(*it == '/' && *std::next(it) == '*')
            {
                ++lexer.comment_nest_level;
                *it++ = ' '; *it   = ' ';
            }
            else if(*it == '*' && *std::next(it) == '/')
            {
                if(lexer.comment_nest_level == 0)
                {
                    lexer.error(begin_pos + std::distance(begin, it), "XXX no multiline comment to close");
                    ++it;
                }
                else
                {
                    --lexer.comment_nest_level;
                    *it++ = ' '; *it   = ' ';
                }
            }
        }

        if(lexer.comment_nest_level)
            *it = ' ';
    }
}

/// Lexes a line.
static void lex_line(LexerContext& lexer, const char* source_data, size_t begin_pos, size_t end_pos)
{
    lexer.line_buffer.assign(source_data + begin_pos, source_data + end_pos);

    bool had_keycommand = false;

    auto begin = (const char*)(&lexer.line_buffer[0]);
    auto end   = begin + lexer.line_buffer.size();
    auto it    = begin;

    auto push_token = [&](const std::pair<const char*, size_t>& token, Token type) -> const char*
    {
        size_t tok_begin = begin_pos + std::distance(begin, token.first);
        lexer.add_token(type, tok_begin, token.second);
        return std::find_if_not(token.first + token.second, end, lex_iswhite);
    };

    auto push_newline = [&]()
    {
        lexer.add_token(Token::NewLine, end_pos, 0);
    };

    lex_comments(lexer, &lexer.line_buffer[0], &lexer.line_buffer[0] + lexer.line_buffer.size(), begin_pos);

    it  = std::find_if_not(it, end, lex_iswhite);
    end = std::find_if_not(std::make_reverse_iterator(end), std::make_reverse_iterator(it), lex_iswhite).base();

    if(std::distance(it, end) == 0)
        return;

    if(lexer.line_buffer.size() > 255)
    {
        // TODO error if pedantic?
    }

    if(auto opt_first_token = lex_gettok(it, end))
    {
        if(opt_first_token->first[opt_first_token->second - 1] == ':')
        {
            it = push_token(*opt_first_token, Token::Label);
        }
    }

    if(auto opt_first_token = lex_gettok(it, end))
    {
        if(lex_istokeq(*opt_first_token, "IF"))
        {
            it = push_token(*opt_first_token, Token::IF);
            had_keycommand = true;
        }
        else if(lex_istokeq(*opt_first_token, "WHILE"))
        {
            it = push_token(*opt_first_token, Token::WHILE);
            had_keycommand = true;
        }
    }

    if(auto opt_first_token = lex_gettok(it, end))
    {
        if(lex_istokeq(*opt_first_token, "AND"))
        {
            it = push_token(*opt_first_token, Token::AND);
            had_keycommand = true;
        }
        else if(lex_istokeq(*opt_first_token, "OR"))
        {
            it = push_token(*opt_first_token, Token::OR);
            had_keycommand = true;
        }
    }

    if(auto opt_first_token = lex_gettok(it, end))
    {
        if(lex_istokeq(*opt_first_token, "NOT"))
        {
            it = push_token(*opt_first_token, Token::NOT);
            had_keycommand = true;
        }
    }

    if(it != end)
    {
        auto begin_expr = it;

        if(lex_isexprc(it, end))
        {
            lex_expr(lexer, begin_expr, end, begin_pos + std::distance(begin, begin_expr));
            push_newline();
            return;
        }
        else
        {
            // <identifier>
            while(it != end && !lex_iswhite(*it) && !lex_isexprc(it, end))
                ++it;

            it = std::find_if_not(it, end, lex_iswhite);

            // <expression_token>
            if(it != end && lex_isexprc(it, end))
            {
                lex_expr(lexer, begin_expr, end, begin_pos + std::distance(begin, begin_expr));
                push_newline();
                return;
            }
        }

        it = begin_expr;
    }

    if(it != end)
    {
        lex_command(lexer, it, end, begin_pos + std::distance(begin, it), had_keycommand);
        push_newline();
        return;
    }

    // it == end
    if(had_keycommand)
    {
        push_newline();
        return;
    }
}

//
// TokenStream
//

std::shared_ptr<TokenStream> TokenStream::tokenize(ProgramContext& program, std::string data_, const char* stream_name)
{
    LexerContext lexer(program, std::move(data_), stream_name);

    auto begin = lexer.stream.data.c_str();
    auto end   = lexer.stream.data.c_str() + lexer.stream.data.size();

    for(auto it = begin; it != end; )
    {
        const char *line_start = it;
        const char *line_end = std::find(it, end, '\n');
        lex_line(lexer, begin, std::distance(begin, line_start), std::distance(begin, line_end));
        it = (line_end == end? line_end : std::next(line_end));
    }

    lexer.verify_nesting();

    if(!lexer.any_error)
        return shared_ptr<TokenStream>(new TokenStream(program, std::move(lexer.stream), std::move(lexer.tokens)));
    else
        return nullptr;
}

std::shared_ptr<TokenStream> TokenStream::tokenize(ProgramContext& program, const fs::path& path)
{
    if(auto opt_data = read_file_utf8(path))
    {
        return TokenStream::tokenize(program, *opt_data, path.generic_u8string().c_str());
    }
    else
    {
        program.error(nocontext, "XXX fail read file {}", path.generic_u8string());
        return nullptr;
    }
}

TokenStream::TokenStream(ProgramContext& program, const char* stream_name, std::string data, std::vector<TokenData> tokens)
    : program(program), text(std::move(data), stream_name), tokens(std::move(tokens))
{
}

TokenStream::TokenStream(ProgramContext& program, TextStream stream, std::vector<TokenData> tokens)
    : program(program), text(std::move(stream)), tokens(std::move(tokens))
{
}

TokenStream::TokenStream(TokenStream&& rhs)
    : program(rhs.program), tokens(std::move(rhs.tokens)), text(std::move(rhs.text))
{
    Ensures(&rhs.program == &this->program);
}

TokenStream::TextStream::TextStream(std::string data_, std::string name_)
    : data(std::move(data_)), stream_name(std::move(name_))
{
    this->max_offset = this->data.size();
    if(!this->data.empty())
    {
        // Assumes an average of 100 characters per line at first.
        this->line_offset.reserve(this->data.size() / 100);

        // pushes first line offset
        this->line_offset.emplace_back(0);

        for(const char* it = this->data.c_str(); *it; ++it)
        {
            if(*it == '\n')
            {
                size_t pos = (it - this->data.c_str());
                this->line_offset.emplace_back(pos+1);
            }
        }

        this->line_offset.shrink_to_fit();
    }
}

std::string TokenStream::TextStream::get_line(size_t lineno) const
{
    size_t offset = offset_for_line(lineno);

    const char* start = this->data.c_str() + offset;
    const char* end;

    for(end = start; *end && *end != '\n' && *end != '\r'; ++end) {}

    return std::string(start, end);
}

size_t TokenStream::TextStream::offset_for_line(size_t lineno) const
{
    size_t i = (lineno - 1);
    if(i < line_offset.size())
        return line_offset[i];
    else
        throw std::logic_error("Bad `lineno` in TokenStream::offset_for_line");
}

auto TokenStream::TextStream::linecol_from_offset(size_t offset) const -> std::pair<size_t, size_t>
{
    for(auto it = line_offset.begin(), end = line_offset.end(); it != end; ++it)
    {
        auto next_it = std::next(it);
        size_t next_line_offset = (next_it != end)? *next_it : this->max_offset;
        
        if(offset >= *it && offset < next_line_offset)
        {
            size_t lineno = size_t(std::distance(line_offset.begin(), it) + 1);
            size_t colno  = (offset - *it) + 1;
            return std::make_pair(lineno, colno);
        }
    }

    throw std::logic_error("bad offset on linecol_from_offset");
}

std::string TokenStream::to_string() const
{
    std::string output;
    for(auto& token : this->tokens)
    {
        auto string = std::string(this->text.data.c_str() + token.begin, this->text.data.c_str() + token.end);
        output += fmt::format("({}) '{}'\n", (int)(token.type), string);
    }
    return output;
}

auto Miss2Identifier::match(const string_view& value) -> expected<Miss2Identifier, std::string>
{
    size_t begin_index = std::string::npos;
    bool is_number_index = true;

    for(size_t i = 0; i < value.size(); ++i)
    {
        if(value[i] == '[')
        {
            if(begin_index != std::string::npos)
                return make_unexpected<std::string>("XXX nesting of arrays index not allowed");

            begin_index = i;
        }
        else if(value[i] == ']')
        {
            auto ident = value.substr(0, begin_index);
            auto index = value.substr(begin_index + 1,  i - (begin_index + 1));
            try
            {
                // TODO check if index positive
                using index_type = decltype(Miss2Identifier::index);
                if(is_number_index)
                    return Miss2Identifier { ident, index_type(std::stoi(index.to_string())) };
                else
                    return Miss2Identifier { ident, index_type(index) };
            }
            catch(const std::out_of_range&)
            {
                return make_unexpected<std::string>("XXX index out of range");
            }
        }
        else if(begin_index != std::string::npos)
        {
            if(value[i] < '0' || value[i] > '9')
                is_number_index = false;
        }
    }

    return Miss2Identifier { value, nullopt };
}
