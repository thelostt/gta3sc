#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

#pragma warning(push)
#pragma warning(disable : 4814) // warning: in C++14 'constexpr' will not imply 'const'; consider explicitly specifying 'const'
#include "cpp/expected.hpp"
#pragma warning(pop)

using token_iterator = std::vector<TokenData>::const_iterator;

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

using ParserResult = expected<std::pair<token_iterator, shared_ptr<SyntaxTree>>, ParserError>;

using ParserRule = ParserResult(*)(ParserContext&, token_iterator, token_iterator);

static ParserResult parser_statement_list(ParserContext& parser, token_iterator begin_, token_iterator end);

static bool parser_istoken(token_iterator it, token_iterator end, Token type)
{
    return (it != end && it->type == type);
}

static ParserResult parser_oneof(ParserContext& parser,
                                 token_iterator begin, token_iterator end)
{
    return make_unexpected<ParserError>(ParserState::GiveUp);
}

template<typename RuleFunc, typename... Args>
static ParserResult parser_oneof(ParserContext& parser,
                                 token_iterator begin, token_iterator end,
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
                                    token_iterator begin, token_iterator end)
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
static ParserResult parser_newline(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::NewLine)
    {
        return std::make_pair(std::next(begin), nullptr);
    }
    return make_unexpected<ParserError>(ParserState::GiveUp);
}

static ParserResult parser_condition_list(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_if_statement(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_argument(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_actual_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_positive_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_statement(ParserContext& parser, token_iterator begin, token_iterator end)
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
static ParserResult parser_statement_list(ParserContext& parser, token_iterator begin_, token_iterator end)
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

static ParserResult parser(ParserContext& parser, token_iterator begin, token_iterator end)
{
    return parser_statement_list(parser, begin, end);
}


//
// SyntaxTree
//

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
