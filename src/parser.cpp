#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

// TODO std::runtime_error to CompilerError or something


struct TokenData
{
    Token  type;
    size_t begin;
    size_t end;
};

struct LexerContext
{
    ProgramContext& program;

    size_t comment_nest_level = 0;
    std::vector<TokenData> tokens;

    std::string temp_line_buffer;
};

static bool lex_iswhite(int c)
{
    return c == ' ' || c == '\t' || c == '(' || c == ')' || c == ',';
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
    auto it = begin;

    while(it != end && lex_iswhite(*it)) ++it;

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
    lexer.tokens.emplace_back(TokenData{ Token::Command, tok_begin, tok_end });
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
}


// Note: Include those after everything else!
// These headers have some bad defines (including true, false and EOF)
#include "grammar/autogen/gta3scriptLexer.h"
#include "grammar/autogen/gta3scriptParser.h"
#undef true
#undef false
#undef EOF

//
// SyntaxTree
//

std::shared_ptr<SyntaxTree> SyntaxTree::from_raw_tree(pANTLR3_BASE_TREE node, const shared_ptr<InputStream>& instream)
{
    std::string data;
    int type = node->getType(node);
    size_t child_count = node->getChildCount(node);

    switch(type)
    {
        case IDENTIFIER:
        {
            data = (const char*)node->getText(node)->chars;
            std::transform(data.begin(), data.end(), data.begin(), ::toupper); // TODO UTF-8 able
            break;
        }

        case INTEGER:
        case FLOAT:
        {
            data = (const char*)node->getText(node)->chars;
            break;
        }

        case STRING:
        {
            data = (const char*) node->getText(node)->chars;
            break;
        }
    }

    std::shared_ptr<SyntaxTree> tree(new SyntaxTree(type, data));
    tree->instream = instream;
    tree->lineno = node->getLine(node);
    tree->colno  = node->getCharPositionInLine(node) + 1;
    tree->childs.reserve(child_count);

    for(size_t i = 0; i < child_count; ++i)
    {
        auto node_child = (pANTLR3_BASE_TREE)(node->getChild(node, i));
        if(node_child->getType(node_child) != SKIPS) // newline statements
        {
            auto my_child = from_raw_tree(node_child, instream);
            my_child->parent_  = std::weak_ptr<SyntaxTree>(tree);
            tree->childs.emplace_back(std::move(my_child));
        }
    }

    return tree;
}

SyntaxTree::SyntaxTree(SyntaxTree&& rhs)
    : type_(rhs.type_), data(std::move(rhs.data)), childs(std::move(rhs.childs)), parent_(std::move(rhs.parent_)),
      udata(std::move(rhs.udata)),
      instream(std::move(rhs.instream)), lineno(rhs.lineno), colno(rhs.colno)
{
    rhs.type_ = NodeType::Ignore;
    rhs.parent_ = nullopt;
}

SyntaxTree& SyntaxTree::operator=(SyntaxTree&& rhs)
{
    this->data = std::move(rhs.data);
    this->childs = std::move(rhs.childs);
    this->type_ = rhs.type_;
    this->parent_ = std::move(rhs.parent_);
    this->udata = std::move(udata);
    this->instream = std::move(rhs.instream);
    this->lineno = rhs.lineno;
    this->colno = rhs.colno;
    rhs.type_ = NodeType::Ignore;
    rhs.parent_ = nullopt;
    return *this;
}

std::shared_ptr<SyntaxTree> SyntaxTree::compile(ProgramContext& program, const TokenStream& tstream)
{
    auto parser = gta3scriptParserNew(tstream.get());
    if(parser)
    {
        auto start_tree = parser->start(parser);

        program.register_errors(parser->pParser->rec->state->errorCount);

        if(start_tree.tree)
        {
            auto instream = std::make_shared<InputStream>();
            instream->filename = std::make_shared<std::string>(tstream.name());

            try
            {
                instream->tstream = tstream.shared_from_this();
            }
            catch(const std::bad_weak_ptr&)
            {
                // cannot get instream->tstream, well, it'll be nullptr then :/
            }

            return SyntaxTree::from_raw_tree(start_tree.tree, instream);
        }
    }

    throw std::runtime_error("TODO error");
}

std::string SyntaxTree::to_string() const
{
    // stub
    return "";
}



//
// TokenStream
//

TokenStream::TokenStream(ProgramContext& program, std::string data_, const char* stream_name)
    : program(program), data(std::move(data_)), sname(stream_name)
{
    this->istream = antlr3StringStreamNew((pANTLR3_UINT8)(data.c_str()), ANTLR3_ENC_UTF8, data.size(), (pANTLR3_UINT8)(stream_name));

    if(istream)
    {
        this->lexer = gta3scriptLexerNew(istream);
        if(this->lexer)
        {
            this->tokstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
            if(this->tokstream)
            {
                this->calc_lines(); // TODO maybe make this call optional?
                return;
            }
        }
    }

    throw std::runtime_error("Failed to open token stream named '" + std::string(stream_name) + "' for reading.");
}

TokenStream::TokenStream(ProgramContext& program, optional<std::string> data_, const char* stream_name)
    : TokenStream(program, (data_? std::move(data_.value()) : throw std::runtime_error("")), stream_name) // TODO simplify this line
{
}

TokenStream::TokenStream(ProgramContext& program, const fs::path& path)
    : TokenStream(program, read_file_utf8(path), path.generic_u8string().c_str())
{
}

TokenStream::TokenStream(TokenStream&& rhs)
    : program(rhs.program), tokstream(nullptr), lexer(nullptr), istream(nullptr)
{
    *this = std::move(rhs);
}

TokenStream& TokenStream::operator=(TokenStream&& rhs)
{
    if(this->tokstream) this->tokstream->free(this->tokstream);
    if(this->lexer) this->lexer->free(this->lexer);
    if(this->istream) this->istream->free(this->istream);

    this->tokstream = rhs.tokstream;
    this->lexer = rhs.lexer;
    this->istream = rhs.istream;

    this->sname = std::move(rhs.sname);
    this->data = std::move(rhs.data);
    this->line_offset = std::move(rhs.line_offset);

    rhs.tokstream = nullptr;
    rhs.lexer = nullptr;
    rhs.istream = nullptr;

    Ensures(&rhs.program == &this->program);

    return *this;
}

TokenStream::~TokenStream()
{
    if(this->tokstream) this->tokstream->free(this->tokstream);
    if(this->lexer) this->lexer->free(this->lexer);
    if(this->istream) this->istream->free(this->istream);
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
