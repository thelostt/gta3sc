#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

// TODO std::runtime_error to CompilerError or something

using TokenData = TokenStream::TokenData;

struct LexerContext
{
    ProgramContext& program;

    size_t comment_nest_level = 0;
    std::vector<TokenData> tokens;

    std::string temp_line_buffer;

    using token_iterator = decltype(tokens)::const_iterator;

    LexerContext(ProgramContext& program) :
        program(program)
    {}
};

static bool lex_iswhite(int c)
{
    return c == ' ' || c == '\t' || c == '(' || c == ')' || c == ',' || c == '\r';
}

static bool lex_isexprc(int c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>';
}

static auto lex_gettok(const char* begin, const char* end) -> optional<std::pair<const char*, size_t>>
{
    const char *tok_start, *tok_end;
    auto it = begin;

    while(it != end && lex_iswhite(*it)) ++it;
    tok_start = it;

    while(it != end && !lex_iswhite(*it)) ++it;
    tok_end = it;

    if(tok_start == tok_end)
        return nullopt;
    return std::make_pair(tok_start, size_t(tok_end - tok_start));
}

static auto lex_skiptok(const char* begin, const char* end) -> const char*
{
    auto it = begin;
    while(it != end && lex_iswhite(*it)) ++it;
    return it;
}


static auto lex_token(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos) -> const char*
{
    while(begin != end && lex_iswhite(*begin)) ++begin, ++begin_pos;

    auto it = begin;

    if(it == end)
        return it;

    switch(*it)
    {
        case '-': /*case '+': TODO '+' not supported by R* compiler, we should perhaps allow but with warning */
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            auto tok_pair = lex_gettok(begin, end).value();
            auto tok_end = (tok_pair.first + tok_pair.second);

            bool is_float = false;
            bool has_error = false;
            for(++it; /**/; ++it)
            {
                if(it == tok_end)
                {
                    Token type = is_float? Token::Float : Token::Integer;
                    lexer.tokens.emplace_back(TokenData{ type, begin_pos, begin_pos + std::distance(begin, it) });
                    return tok_end;
                }

                // TODO R* compiler doesn't allow hex and octal values, we should?
                // TODO R* compiler allows '-' anywhere, we should emit a warning.
                // TODO if float, R* compiler doesn't allow '-' anywhere.
                if((*it >= '0' && *it <= '9') || *it == '-')
                    continue;

                // TODO R* compiler allows the f|F|. more than once, but we should emit a warning.
                // TODO custom exponent and such
                if(*it == '.' || *it == 'f' || *it == 'F')
                {
                    is_float = true;
                    continue;
                }

                // bad suffix
                lexer.program.error(nocontext, "XXX bad suffix '{}' on int const", std::string(it, tok_end));
                return tok_end;
            }
            Unreachable();
        }

        case '"':
        {
            do
            {
                if(++it == end)
                {
                    lexer.program.error(nocontext, "XXX end of line without closing quotes");
                    return it;
                }
            }
            while(*it != '"');

            return ++it;
        }

        default:
        {
            auto tok_pair = lex_gettok(begin, end).value();

            if((*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z') || *it == '$')
            {
                it = tok_pair.first + tok_pair.second;
                lexer.tokens.emplace_back(TokenData{ Token::Identifier, begin_pos, begin_pos + std::distance(begin, it) });
                return it;
            }
            else
            {
                lexer.program.error(nocontext, "XXX bad ident");
                return tok_pair.first + tok_pair.second;
            }
            break;
        }
    }
}

static void lex_expr(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos)
{
    // TODO
}

static void lex_command(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos, bool had_keycommand)
{
    struct KeyCommand
    {
        const char* keyword;
        size_t      length;
        Token       token;
    };

    #define DEFINE_KEYCOMMAND(kw)       KeyCommand { #kw, sizeof(#kw) - 1, Token::##kw }
    #define DEFINE_KEYSYMBOL(sym, tok)  KeyCommand { #sym, sizeof(#sym) - 1, tok }
    static const KeyCommand keycommands[] = {
        DEFINE_KEYSYMBOL("{", Token::ScopeBegin),
        DEFINE_KEYSYMBOL("}", Token::ScopeEnd),
        //DEFINE_KEYCOMMAND(NOT),
        //DEFINE_KEYCOMMAND(AND),
        //DEFINE_KEYCOMMAND(OR),
        //DEFINE_KEYCOMMAND(IF),
        DEFINE_KEYCOMMAND(ELSE),
        DEFINE_KEYCOMMAND(ENDIF),
        //DEFINE_KEYCOMMAND(WHILE),
        DEFINE_KEYCOMMAND(ENDWHILE),
        DEFINE_KEYCOMMAND(REPEAT),
        DEFINE_KEYCOMMAND(ENDREPEAT),
        DEFINE_KEYCOMMAND(SWITCH),
        DEFINE_KEYCOMMAND(ENDSWITCH),
        DEFINE_KEYCOMMAND(CASE),
        DEFINE_KEYCOMMAND(DEFAULT),
        DEFINE_KEYCOMMAND(BREAK),
        DEFINE_KEYCOMMAND(CONTINUE),
        DEFINE_KEYCOMMAND(MISSION_START),
        DEFINE_KEYCOMMAND(MISSION_END),
        DEFINE_KEYCOMMAND(SCRIPT_START),
        DEFINE_KEYCOMMAND(SCRIPT_END),
        DEFINE_KEYCOMMAND(VAR_INT),
        DEFINE_KEYCOMMAND(LVAR_INT),
        DEFINE_KEYCOMMAND(VAR_FLOAT),
        DEFINE_KEYCOMMAND(LVAR_FLOAT),
        DEFINE_KEYCOMMAND(VAR_TEXT_LABEL),
        DEFINE_KEYCOMMAND(LVAR_TEXT_LABEL),
        DEFINE_KEYCOMMAND(VAR_TEXT_LABEL16),
        DEFINE_KEYCOMMAND(LVAR_TEXT_LABEL16),
    };
    #undef DEFINE_KEYCOMMAND
    #undef DEFINE_KEYSYMBOL

    auto it = begin;
    if(auto opt_cmdtok = lex_gettok(it, end))
    {
        it = opt_cmdtok->first + opt_cmdtok->second;

        bool is_keycommand = false;

        for(auto& keycmd : keycommands)
        {
            if(keycmd.length == opt_cmdtok->second
            && !strncasecmp(opt_cmdtok->first, keycmd.keyword, keycmd.length))
            {
                // TODO if had_keycommand, error
                size_t tok_begin = begin_pos + std::distance(begin, opt_cmdtok->first);
                size_t tok_end   = tok_begin + opt_cmdtok->second;
                lexer.tokens.emplace_back(TokenData{ keycmd.token, tok_begin, tok_end });
                is_keycommand = true;
                break;
            }
        }

        if(!is_keycommand)
        {
            size_t tok_begin = begin_pos + std::distance(begin, opt_cmdtok->first);
            size_t tok_end   = tok_begin + opt_cmdtok->second;
            lexer.tokens.emplace_back(TokenData{ Token::Command, tok_begin, tok_end });
        }

        while(it != end)
        {
            it = lex_token(lexer, it, end, begin_pos + std::distance(begin, it));
        }
    }
}

static void lex_newline(LexerContext& lexer, const char* begin, const char* end, size_t begin_pos)
{
    size_t tok_begin = begin_pos + std::distance(begin, end);
    size_t tok_end   = tok_begin;
    lexer.tokens.emplace_back(TokenData{ Token::NewLine, tok_begin, tok_end });
}

static void lex_line(LexerContext& lexer, const char* text_ptr, size_t begin_pos, size_t end_pos)
{
    char *buffer_ptr, temp_line_buffer[256];
    size_t line_length = end_pos - begin_pos;
    bool had_keycommand = false;

    if(line_length == 0)
        return;  // TODO push NL to tokens

    if(line_length > 255)
    {
        // TODO error if pedantic?
    }

    if(line_length < std::size(temp_line_buffer))
    {
        std::memcpy(temp_line_buffer, text_ptr + begin_pos, line_length);
        buffer_ptr = &temp_line_buffer[0];
    }
    else
    {
        lexer.temp_line_buffer.assign(text_ptr + begin_pos, text_ptr + end_pos);
        buffer_ptr = &lexer.temp_line_buffer[0];
    }

    // trim comments
    if(true)
    {
        bool in_quotes = false;

        for(size_t i = 0; i < line_length; ++i)
        {
            if(lexer.comment_nest_level == 0 && buffer_ptr[i] == '"')
            {
                in_quotes = !in_quotes;
            }
            else if(i+1 != line_length)
            {
                if(in_quotes)
                {
                    /* TODO not pedantic
                    if(buffer_ptr[i] == '\\' && buffer_ptr[i+1] == '"')
                        ++i;
                    */
                }
                else if(buffer_ptr[i] == '/' && buffer_ptr[i+1] == '/')
                {
                    line_length = i++;
                    break;
                }
                else if(buffer_ptr[i] == '/' && buffer_ptr[i+1] == '*')
                {
                    ++lexer.comment_nest_level;
                    buffer_ptr[i++] = ' ';
                    buffer_ptr[i]   = ' ';
                }
                else if(buffer_ptr[i] == '*' && buffer_ptr[i+1] == '/')
                {
                    // TODO check if comment_nest_level == 0 and error
                    --lexer.comment_nest_level;
                    buffer_ptr[i++] = ' ';
                    buffer_ptr[i]   = ' ';
                }
            }

            if(lexer.comment_nest_level)
            {
                buffer_ptr[i] = ' ';
            }
        }
    }

    //////////

    const char* const_buffer_ptr = buffer_ptr;
    auto it = const_buffer_ptr;
    auto end = const_buffer_ptr + line_length;

    // skip front whitespaces
    while(it != end && lex_iswhite(*it)) ++it;

    // Try label token
    if(it != end)
    {
        if(auto opt_first_token = lex_gettok(it, end))
        {
            auto& first_token = *opt_first_token;
            if(first_token.first[first_token.second - 1] == ':')
            {
                size_t label_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t label_end   = label_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData { Token::Label, label_begin, label_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
            }
        }
    }

    // Try WHILE / WHILENOT / IF / IFNOT (must be before AND/OR/NOT)
    if(it != end)
    {
        if(auto opt_first_token = lex_gettok(it, end))
        {
            auto& first_token = *opt_first_token;

            if(!strncasecmp(first_token.first, "IF", first_token.second))
            {
                size_t tok_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t tok_end   = tok_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData{ Token::IF, tok_begin, tok_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
                had_keycommand = true;
            }
            else if(!strncasecmp(first_token.first, "WHILE", first_token.second))
            {
                size_t tok_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t tok_end   = tok_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData{ Token::WHILE, tok_begin, tok_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
                had_keycommand = true;
            }
        }
    }

    // Try AND / OR
    if(it != end)
    {
        if(auto opt_first_token = lex_gettok(it, end))
        {
            auto& first_token = *opt_first_token;

            if(!strncasecmp(first_token.first, "AND", first_token.second))
            {
                size_t tok_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t tok_end   = tok_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData{ Token::AND, tok_begin, tok_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
                had_keycommand = true;
            }
            else if(!strncasecmp(first_token.first, "OR", first_token.second))
            {
                size_t tok_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t tok_end   = tok_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData{ Token::OR, tok_begin, tok_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
                had_keycommand = true;
            }
        }
    }

    // Try NOT
    if(it != end)
    {
        if(auto opt_first_token = lex_gettok(it, end))
        {
            auto& first_token = *opt_first_token;

            if(!strncasecmp(first_token.first, "NOT", first_token.second))
            {
                size_t tok_begin = begin_pos + std::distance(const_buffer_ptr, first_token.first);
                size_t tok_end   = tok_begin + first_token.second;
                lexer.tokens.emplace_back(TokenData{ Token::NOT, tok_begin, tok_end });

                // Skip this token
                it = lex_skiptok(first_token.first + first_token.second, end);
                had_keycommand = true;
            }
        }
    }

    // Try expression context
    if(it != end)
    {
        auto begin_expr = it;

        if(lex_isexprc(*it))
        {
            lex_expr(lexer, begin_expr, end, begin_pos + std::distance(const_buffer_ptr, begin_expr));
            lex_newline(lexer, begin_expr, end, begin_pos + std::distance(const_buffer_ptr, begin_expr));
            return;
        }

        auto tok_begin = it;
        while(it != end && !lex_iswhite(*it) && !lex_isexprc(*it)) ++it;
        auto tok_end = it;

        assert(tok_begin != tok_end);

        if(it != end)
        {
            while(it != end && lex_iswhite(*it)) ++it;

            if(it != end && lex_isexprc(*it))
            {
                lex_expr(lexer, begin_expr, end, begin_pos + std::distance(const_buffer_ptr, begin_expr));
                lex_newline(lexer, begin_expr, end, begin_pos + std::distance(const_buffer_ptr, begin_expr));
                return;
            }
        }

        // Fall back to the original iterator value
        it = begin_expr;
    }

    // Try command context
    if(it != end)
    {
        lex_command(lexer, it, end, begin_pos + std::distance(const_buffer_ptr, it), had_keycommand);
        lex_newline(lexer, it, end, begin_pos + std::distance(const_buffer_ptr, it));
        return;
    }
}

static void lex(LexerContext& lexer, const char* begin, const char* end)
{
    for(auto it = begin; it != end; it != end? ++it : it)
    {
        const char *line_start = it;

        while(it != end && *it != '\n')
            ++it;

        lex_line(lexer, begin, std::distance(begin, line_start), std::distance(begin, it));
    }

    // EOF with a NEWLINE token
    lex_newline(lexer, end, end, std::distance(begin, end));
}

//
// TokenStream
//

std::shared_ptr<TokenStream> TokenStream::tokenize(ProgramContext& program, std::string data, const char* stream_name)
{
    // TODO check for errors?!
    LexerContext lexer(program);
    lex(lexer, data.c_str(), data.c_str() + data.size());

    // Use new instead of make_shared, we don't want weak_ptr to leave the TokenStream on memory.
    return shared_ptr<TokenStream>(new TokenStream(program, stream_name, std::move(data), std::move(lexer.tokens)));

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
    : program(program), data(std::move(data)), stream_name(stream_name), tokens(std::move(tokens))
{
    this->calc_lines();
}

TokenStream::TokenStream(TokenStream&& rhs)
    : program(rhs.program), tokens(std::move(rhs.tokens)), line_offset(std::move(line_offset)),
      stream_name(std::move(rhs.stream_name)), data(std::move(rhs.data))
{
    Ensures(&rhs.program == &this->program);
}

void TokenStream::calc_lines()
{
    if(this->data.empty() || !this->line_offset.empty())
        return;

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

std::string TokenStream::get_line(size_t lineno) const
{
    size_t offset = offset_for_line(lineno);

    const char* start = this->data.c_str() + offset;
    const char* end;

    for(end = start; *end && *end != '\n' && *end != '\r'; ++end) {}

    return std::string(start, end);
}

size_t TokenStream::offset_for_line(size_t lineno) const
{
    size_t i = (lineno - 1);
    if(i < line_offset.size())
        return line_offset[i];
    else
        throw std::logic_error("Bad `lineno` in TokenStream::offset_for_line");
}

auto TokenStream::linecol_from_offset(size_t offset) const -> std::pair<size_t, size_t>
{
    for(auto it = line_offset.begin(), end = line_offset.end(); it != end; ++it)
    {
        auto next_it = std::next(it);
        size_t next_line_offset = (next_it != end)? *next_it : SIZE_MAX;
        
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
        output += fmt::format("({}) '{}'\n", (int)(token.type), std::string(this->data.c_str() + token.begin, this->data.c_str() + token.end));
    }
    return output;
}
