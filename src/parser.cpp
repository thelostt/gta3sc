#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

#pragma warning(push)
#pragma warning(disable : 4814) // warning: in C++14 'constexpr' will not imply 'const'; consider explicitly specifying 'const'
#include "cpp/expected.hpp"
#pragma warning(pop)

// TODO std::runtime_error to CompilerError or something




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

//////////////////////////////////////////////////////

struct ParserContext
{
    ProgramContext&                      program;
    const TokenStream&                   tstream;
    shared_ptr<SyntaxTree::InputStream>  instream;

    ParserContext(ProgramContext& program, const TokenStream& tstream) :
        program(program), tstream(tstream)
    {
        this->instream = std::make_shared<SyntaxTree::InputStream>();
        this->instream->filename = std::make_shared<std::string>(tstream.name());
        this->instream->tstream = tstream.shared_from_this();
    }
};

enum class ParserState
{
    GiveUp,         // Could not understand a thing (e.g. SWITCH while trying to parse WHILE).
    //ExpectedOneOf,  // Returned by parser_oneof if none of the rules were matched.
    Error,
};

struct ParserError
{
    ParserState state;
    std::string error;

    ParserError(ParserState state) :
        state(state)
    {}

    explicit ParserError(ParserState state, std::string error) :
        state(state), error(std::move(error))
    {}
};

using ParserResult = expected<std::pair<LexerContext::token_iterator, shared_ptr<SyntaxTree>>, ParserError>;

using ParserRule = ParserResult(*)(ParserContext&, LexerContext::token_iterator, LexerContext::token_iterator);

static ParserResult parser_statement_list(ParserContext& parser, LexerContext::token_iterator begin_, LexerContext::token_iterator end);

static bool parser_istoken(LexerContext::token_iterator it, LexerContext::token_iterator end, Token type)
{
    return (it != end && it->type == type);
}

static ParserResult parser_oneof(ParserContext& parser,
                                 LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    return make_unexpected<ParserError>(ParserState::GiveUp);
}

template<typename RuleFunc, typename... Args>
static ParserResult parser_oneof(ParserContext& parser,
                                 LexerContext::token_iterator begin, LexerContext::token_iterator end,
                                 RuleFunc rule, Args&&... rules)
{

    ParserResult opt_result = rule(parser, begin, end);
    if(opt_result)
        return opt_result;
    else if(opt_result.error().state != ParserState::GiveUp)
        return opt_result; // error state
    else
        return parser_oneof(parser, begin, end, std::forward<Args>(rules)...);
}

// success or failure, never GiveUp
template<typename RuleFunc>
static ParserResult parser_expected(RuleFunc rule,
                                    ParserContext& parser,
                                    LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    if(begin == end)
        return make_unexpected(ParserError(ParserState::Error, "XXX error expected rule found end"));

    if(auto opt_result = rule(parser, begin, end))
        return opt_result;

    return make_unexpected(ParserError(ParserState::Error, "XXX error expected rule"));
}

/*
    newLine
	    :	WS* '\r'? ('\n'|EOF) -> SKIPS
	    ;
*/
static ParserResult parser_newline(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    if(begin->type == Token::NewLine)
    {
        return std::make_pair(std::next(begin), nullptr);
    }
    return make_unexpected<ParserError>(ParserState::GiveUp);
}

static ParserResult parser_condition_list(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    // TODO
}

/*
    ifStatement
	    :	WS* IF conditionList
			    statIf=statementList
		    (WS* ELSE newLine
			    statElse=statementList)?
		    WS* ENDIF newLine
	    ->	^(IF conditionList $statIf ^(ELSE $statElse)?)
	    ;
*/
static ParserResult parser_if_statement(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    Unreachable(); // TODO REMOVE
#if 0
    auto it = begin;

    if(it->type == Token::IF)
    {
        ParserResult opt_expect;

        ParserResult opt_clist = parser_expected(parser_condition_list, parser, std::next(it), end);

        if(opt_clist)
        {
            ParserResult opt_statif = parser_statement_list(parser, it, end);
            optional<ParserResult> opt_statelse;

            if(opt_statif)
            {
                it = opt_statif->first;
                if(parser_istoken(it, end, Token::ELSE))
                {
                    opt_expect = parser_expected(parser_newline, parser, std::next(it), end);
                    if(!opt_expect) return opt_expect;

                    it = opt_expect->first;
                    *opt_statelse = parser_statement_list(parser, it, end);
                    if(!*opt_statelse) return *opt_statelse;
                    it = opt_expect->first;
                }

                if(parser_istoken(it, end, Token::ENDIF))
                {
                    opt_expect = parser_expected(parser_newline, parser, std::next(it), end);
                    if(!opt_expect) return opt_expect;
                    it = opt_expect->first;
                }

                shared_ptr<SyntaxTree> if_tree(new SyntaxTree(NodeType::IF));
                if_tree->add_child(opt_clist->second);
                if_tree->add_child(opt_statif->second);
                if(opt_statelse)
                {
                    shared_ptr<SyntaxTree> else_tree(new SyntaxTree(NodeType::ELSE));
                    else_tree->add_child((*opt_statelse)->second);
                    if_tree->add_child(std::move(else_tree));
                }

                return std::make_pair(it, if_tree);
            }
            else
                return opt_statif; // error state
        }
        else
            return opt_clist;  // error state
    }

    return make_unexpected<ParserError>(ParserState::GiveUp);
#endif
}

/*
    argument
	    :	WS*
		    (INTEGER -> INTEGER
		    |FLOAT -> FLOAT
		    |IDENTIFIER '[' idx=(IDENTIFIER|INTEGER) WS* ']' -> ^(ARRAY IDENTIFIER $idx)
		    |IDENTIFIER -> IDENTIFIER
		    |STRING -> STRING)
	    ;
*/
static ParserResult parser_argument(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    // TODO array? probably not
    if(begin->type == Token::Integer)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Integer, parser.instream, *begin));
        return std::make_pair(std::next(begin), node);
    }
    else if(begin->type == Token::Float)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Float, parser.instream, *begin));
        return std::make_pair(std::next(begin), node);
    }
    else if(begin->type == Token::Identifier)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Identifier, parser.instream, *begin));
        return std::make_pair(std::next(begin), node);
    }
    else if(begin->type == Token::String)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::String, parser.instream, *begin));
        return std::make_pair(std::next(begin), node);
    }
    return make_unexpected<ParserError>(ParserState::GiveUp);
}

// (WS* IDENTIFIER argument* newLine) -> ^(COMMAND IDENTIFIER argument*)
static ParserResult parser_actual_command_statement(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    auto it = begin;
    if(it->type == Token::Command)
    {
        shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Command, parser.instream, *it));
        tree->add_child(shared_ptr<SyntaxTree> { new SyntaxTree(NodeType::Identifier, parser.instream, *it) });

        for(++it; it != end; ++it)
        {
            if(it->type == Token::NewLine) // TODO to call or not to call parser_newline instead of manual compare?
                break;

            ParserResult opt_arg = parser_argument(parser, it, end);
            if(opt_arg)
            {
                it = opt_arg->first;
                tree->add_child(opt_arg->second);
            }
            else
            {
                Expects(opt_arg.error().state == ParserState::GiveUp);
                return make_unexpected(ParserError { ParserState::Error, "XXX parser error expected argument" });
            }
        }

        return std::make_pair(it, tree);
    }

    return make_unexpected<ParserError>(ParserState::GiveUp);
}


/*
    positiveCommandStatement
	    :	expressionStatement
	    |	(WS* IDENTIFIER argument* newLine) -> ^(COMMAND IDENTIFIER argument*)
	    ;
*/
static ParserResult parser_positive_command_statement(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    ParserResult opt_result = parser_oneof(parser, begin, end,
                                           //parser_expression_statement, TODO
                                           parser_actual_command_statement);

    if(opt_result)
        return opt_result;
    else if(opt_result.error().state == ParserState::GiveUp)
        return make_unexpected(ParserError(ParserState::GiveUp, "XXX error"));
    else
        return opt_result;  // error state
}

/*
    commandStatement
	    :	WS* NOT positiveCommandStatement -> ^(NOT positiveCommandStatement)
	    |	positiveCommandStatement -> positiveCommandStatement
	    ;
*/
static ParserResult parser_command_statement(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    auto it = begin;
    const TokenData* it_not_token = nullptr;

    bool is_not = (it->type == Token::NOT);
    if(is_not)
    {
        it_not_token = std::addressof(*it);

        if(++it == end)
            return make_unexpected(ParserError(ParserState::Error, "XXX error"));
    }

    auto opt_positive = parser_positive_command_statement(parser, begin, end);
    if(opt_positive)
    {
        if(is_not)
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::NOT, parser.instream, *it_not_token));
            tree->add_child(opt_positive->second);
            return std::make_pair(opt_positive->first, tree);
        }
        else
        {
            return opt_positive;
        }
    }
    else
        return opt_positive;  // error state
}




/*
    statement
	    : newLine
	    | scopeStatement
	    | ifStatement
	    | whileStatement
	    | repeatStatement
	    | switchStatement
	    | variableDeclaration
	    | loopControlStatement
	    | labelStatement
	    | keyStatement
	    | commandStatement
	    ;
*/
// ParserResult->second may be nullptr (e.g. in case of newline), just ignore such node
static ParserResult parser_statement(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    ParserResult opt_result = parser_oneof(parser, begin, end,
                                           parser_newline,
                                           parser_command_statement);
    if(opt_result)
        return opt_result;
    else if(opt_result.error().state == ParserState::GiveUp)
        return make_unexpected(ParserError(ParserState::GiveUp, "XXX error"));
    else
        return opt_result;  // error state
}

/*
    statementList
	    :	statement*
	    ->  ^(BLOCK statement*)
	    ;
*/
static ParserResult parser_statement_list(ParserContext& parser, LexerContext::token_iterator begin_, LexerContext::token_iterator end)
{
    ParserResult opt_statement = std::make_pair(begin_, nullptr);
    shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Block, parser.instream, *begin_));

    while(opt_statement->first != end)
    {
        if(opt_statement = parser_statement(parser, opt_statement->first, end))
        {
            if(opt_statement->second)
            {
                tree->add_child(opt_statement->second);
            }
        }
        else
        {
            return opt_statement; // error state
        }
    }

    return std::make_pair(opt_statement->first, tree);
}

static ParserResult parser(ParserContext& parser, LexerContext::token_iterator begin, LexerContext::token_iterator end)
{
    return parser_statement_list(parser, begin, end);
}


//////////////////////////////////////////////////////


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
    /* TODO
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
    */
    Unreachable(); // unimplemented
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
    ParserContext parser_ctx(program, tstream);

    ParserResult presult = parser(parser_ctx, tstream.tokens.begin(), tstream.tokens.end());

    if(presult)
    {
        puts(presult->second->to_string().c_str());
        puts("---------------------------\n");
        throw 0; // TODO remove me
        return presult->second;
    }
    else
    {
        throw 1; // TODO remove me
        program.error(nocontext, "XXX PARSER ERROR");
        return nullptr;
    }
}

std::string SyntaxTree::to_string(size_t level) const
{
    std::string output = fmt::format("{:>{}} ({}) {}\n", ' ', level * 4, int(this->type()), this->text());

    for(auto& child : *this)
    {
        output += child->to_string(level + 1);
    }

    return output;
}



//
// TokenStream
//

TokenStream::TokenStream(ProgramContext& program, std::string data_, const char* stream_name)
    : program(program), data(std::move(data_)), sname(stream_name)
{
    // TODO check for errors?!
    LexerContext lexer(program);
    lex(lexer, this->data.c_str(), this->data.c_str() + this->data.size());
    this->tokens = std::move(lexer.tokens);
    this->calc_lines(); // TODO maybe make this call optional?

    puts(this->to_string().c_str());
    puts("---------------------------\n");
    //throw 0;//TODO REMOVE ME
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
    : program(rhs.program)
{
    *this = std::move(rhs);
}

TokenStream& TokenStream::operator=(TokenStream&& rhs)
{
    this->tokens = std::move(rhs.tokens);
    this->sname = std::move(rhs.sname);
    this->data = std::move(rhs.data);
    this->line_offset = std::move(rhs.line_offset);

    Ensures(&rhs.program == &this->program);

    return *this;
}

TokenStream::~TokenStream()
{
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

std::string TokenStream::to_string() const
{
    std::string output;
    for(auto& token : this->tokens)
    {
        output += fmt::format("({}) '{}'\n", (int)(token.type), std::string(this->data.c_str() + token.begin, this->data.c_str() + token.end));
    }
    return output;
}
