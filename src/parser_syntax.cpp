#include "stdinc.h"
#include "parser.hpp"
#include "program.hpp"

using TokenData      = TokenStream::TokenData;
using token_iterator = const TokenData*;//std::vector<TokenData>::const_iterator;

enum class ParserStatus
{
    GiveUp,         // Could not understand a thing (e.g. SWITCH while trying to parse WHILE).
    Error,
};

struct ParserSuccess
{
    shared_ptr<SyntaxTree> tree;

    explicit ParserSuccess(shared_ptr<SyntaxTree> tree) :
        tree(std::move(tree))
    {}

    explicit ParserSuccess(SyntaxTree* raw_tree_2smart) :
        tree(raw_tree_2smart)
    {}

    explicit ParserSuccess(std::nullptr_t)
    {}
};

struct ParserError
{
    ParserStatus     state;
    std::string     error;
    token_iterator  it_stop;

    ParserError(ParserStatus state) :
        state(state)
    {}

    explicit ParserError(ParserStatus state, token_iterator it_stop, std::string error) :
        state(state), it_stop(std::move(it_stop)), error(std::move(error))
    {}
};

struct ParserContext
{
    ProgramContext&                      program;
    const TokenStream&                   tstream;
    shared_ptr<SyntaxTree::InputStream>  instream;

    ParserContext(ProgramContext& program, const TokenStream& tstream) :
        program(program), tstream(tstream)
    {
        this->instream = std::make_shared<SyntaxTree::InputStream>();
        this->instream->filename = std::make_shared<std::string>(tstream.stream_name);
        this->instream->tstream = tstream.shared_from_this();
    }
};


using ParserFailure = small_vector<ParserError, 1>;

using ParserState = variant<ParserSuccess, ParserFailure>;

using ParserResult = std::pair<token_iterator, ParserState>;

using ParserRule = ParserResult(*)(ParserContext&, token_iterator, token_iterator);

//static ParserResult parser_statement_list(ParserContext& parser, token_iterator begin_, token_iterator end);
static ParserResult parse_command_statement(ParserContext& parser, token_iterator begin, token_iterator end);
static ParserResult parse_statement(ParserContext& parser, token_iterator begin, token_iterator end);

static auto make_error(ParserError error) -> ParserState
{
    return ParserState(ParserFailure { std::move(error) });
}

static void add_error(ParserState& opt, ParserError e)
{
    if(is<ParserSuccess>(opt))
        opt = make_error(std::move(e));
    else
        get<ParserFailure>(opt).emplace_back(std::move(e));
}

static void add_error(ParserState& opt, const ParserFailure& elist)
{
    for(auto& e : elist)
        add_error(opt, e);
}

static void add_error(ParserState& opt, const ParserState& estate)
{
    if(!is<ParserSuccess>(estate))
        add_error(opt, get<ParserFailure>(estate));
}

static bool parser_isgiveup(const ParserState& opt)
{
    if(!is<ParserSuccess>(opt))
    {
        for(auto& e : get<ParserFailure>(opt))
        {
            if(e.state != ParserStatus::GiveUp)
                return false;
        }
        assert(get<ParserFailure>(opt).size() == 1); // this is not necessarely true, 
                                         // if this asserts, just remove this line.
        return true;
    }
    return false;
}

// requires std::prev(it) to be valid
static bool expect_newline(ParserState& state, token_iterator it, token_iterator end)
{
    if(it != end && it->type != Token::NewLine)
    {
        add_error(state, make_error(ParserError(ParserStatus::Error, std::prev(it), "XXX expected newline after this token")));
        return false;
    }
    return true;
}

static ParserState giveup_to_expected(ParserState state, const char* what)
{
    if(!is<ParserSuccess>(state))
    {
        ParserState new_state = ParserSuccess(nullptr);
        for(auto& e : get<ParserFailure>(state))
        {
            if(e.state == ParserStatus::GiveUp)
                add_error(new_state, ParserError(ParserStatus::Error, e.it_stop, fmt::format("expected {}", what)));
            else
                add_error(new_state, std::move(e)); 
        }
        return new_state;
    }
    return state;
}

// returns it with std::prev(it)->type == type or end
static token_iterator parser_aftertoken(token_iterator it, token_iterator end, Token type)
{
    for(; it != end; ++it)
    {
        if(it->type == type)
            return std::next(it);
    }
    return end;
}

static ParserResult parse_oneof(ParserContext& parser,
                                 token_iterator begin, token_iterator end)
{
    return std::make_pair(end, make_error(ParserError(ParserStatus::GiveUp, begin, "")));
}

template<typename RuleFunc, typename... Args>
static ParserResult parse_oneof(ParserContext& parser,
                                 token_iterator begin, token_iterator end,
                                 RuleFunc rule, Args&&... rules)
{

    ParserResult result = rule(parser, begin, end);
    if(!parser_isgiveup(result.second))
        return result;
    else
        return parse_oneof(parser, begin, end, std::forward<Args>(rules)...);

}


// Returns either success, if the rule returns ParserSuccess until condition happens,
// or returns failure, with all the ParserFailure states untouched (including GiveUp), in the result state.
template<typename RuleFunc, typename UntilCondition>
static ParserResult parse_until_if(RuleFunc rule,
                                   ParserContext& parser, token_iterator begin, token_iterator end,
                                   UntilCondition cond)
{
    shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Block));
    auto it = begin;

    ParserState state = ParserSuccess(nullptr);

    for(; it != end && !cond(it); )
    {
        auto rule_result = rule(parser, it, end);
        if(is<ParserSuccess>(rule_result.second))
        {
            if(is<ParserSuccess>(state))
                tree->add_child(get<ParserSuccess>(rule_result.second).tree);
        }
        else
        {
            add_error(state, std::move(get<ParserFailure>(rule_result.second)));
        }

        it = rule_result.first;
    }

    if(is<ParserSuccess>(state))
        return std::make_pair(it, ParserSuccess(std::move(tree)));
    else
        return std::make_pair(it, std::move(state));
}






//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////



/*
    newLine
	    :	WS* '\r'? ('\n'|EOF) -> SKIPS
	    ;
*/
static ParserResult parse_newline(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::NewLine)
    {
        return std::make_pair(std::next(begin), ParserSuccess(new SyntaxTree(NodeType::Ignore, parser.instream, *begin)));
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

static ParserResult parse_identifier(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::Identifier)
    {
        return std::make_pair(std::next(begin), ParserSuccess(new SyntaxTree(NodeType::Identifier, parser.instream, *begin)));
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

/*
    scopeStatement
	    :	WS* '{' newLine
			    statementList
		    WS* '}' newLine
	    ->	^(SCOPE statementList)
	    ;
*/
static ParserResult parse_scope_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::ScopeBegin)
    {
        auto it = std::next(begin);

        ParserState state = ParserSuccess(nullptr);
        ParserState statements;

        expect_newline(state, it, end);
        it = parser_aftertoken(it, end, Token::NewLine);

        std::tie(it, statements) = parse_until_if(parse_statement, parser, it, end, [](token_iterator t) {
            return t->type == Token::ScopeEnd;
        });

        if(it == end)
            add_error(state, ParserError(ParserStatus::Error, begin, "XXX unmatched '}}' for '{{'"));
        else
        {
            Expects(it->type == Token::ScopeEnd);
            ++it;

            expect_newline(state, it, end);
            it = parser_aftertoken(it, end, Token::NewLine);
        }

        add_error(state, statements); // if is<ParserFailure>(statements)

        if(is<ParserSuccess>(state))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Scope, parser.instream, *begin));
            tree->add_child(get<ParserSuccess>(statements).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, giveup_to_expected(std::move(state), "statement"));
        }
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
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
static ParserResult parse_argument(ParserContext& parser, token_iterator begin, token_iterator end)
{
    // TODO array? probably not
    if(begin->type == Token::Integer)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Integer, parser.instream, *begin));
        return std::make_pair(std::next(begin), ParserSuccess(std::move(node)));
    }
    else if(begin->type == Token::Float)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Float, parser.instream, *begin));
        return std::make_pair(std::next(begin), ParserSuccess(std::move(node)));
    }
    else if(begin->type == Token::Identifier)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::Identifier, parser.instream, *begin));
        return std::make_pair(std::next(begin), ParserSuccess(std::move(node)));
    }
    else if(begin->type == Token::String)
    {
        shared_ptr<SyntaxTree> node(new SyntaxTree(NodeType::String, parser.instream, *begin));
        return std::make_pair(std::next(begin), ParserSuccess(std::move(node)));
    }
    return std::make_pair(std::next(begin), make_error(ParserStatus::GiveUp));
}

/*
    expressionStatement
	    :	(id=identifier opa1=assignmentOperators1 a=argument newLine)
		    ->  ^($opa1 $id $a)
	    |	(id=identifier opa2=assignmentOperators2 a=argument newLine)
		    ->  ^(OP_EQ $id ^($opa2  ^($id) $a))
		    // for some reason $id (instead of ^($id)) doesn't work properly with arrays
	    |	(id=identifier WS* '=' a=argument opb=binaryOperators b=argument newLine)
		    ->  ^(OP_EQ $id ^($opb $a $b))
	    |	(a=argument opr=relationalOperators b=argument newLine)
		    ->  ^($opr $a $b)
	    |   ((unaryOperators identifier | identifier unaryOperators) newLine)
		    ->  ^(unaryOperators identifier)
	    ;
*/
static ParserResult parse_expression_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    auto assignment_operators1 = [](Token token) -> optional<NodeType>
    {
        switch(token)
        {
            case Token::Equal: return NodeType::Equal;
            case Token::Cast:  return NodeType::Cast;
            default:           return nullopt;
        }
    };

    auto assignment_operators2 = [](Token token) -> optional<NodeType>
    {
        switch(token)
        {
            case Token::EqPlus:         return NodeType::Plus;
            case Token::EqMinus:        return NodeType::Minus;
            case Token::EqTimes:        return NodeType::Times;
            case Token::EqDivide:       return NodeType::Divide;
            case Token::EqTimedPlus:    return NodeType::TimedPlus;
            case Token::EqTimedMinus:   return NodeType::TimedMinus;
            default:                    return nullopt;
        }
    };

    auto binary_operators = [](Token token) -> optional<NodeType>
    {
        switch(token)
        {
            case Token::Plus:           return NodeType::Plus;
            case Token::Minus:          return NodeType::Minus;
            case Token::Times:          return NodeType::Times;
            case Token::Divide:         return NodeType::Divide;
            case Token::TimedPlus:      return NodeType::TimedPlus;
            case Token::TimedMinus:     return NodeType::TimedMinus;
            default:                    return nullopt;
        }
    };

    auto relational_operators = [](Token token) -> optional<NodeType>
    {
        switch(token)
        {
            case Token::Lesser:         return NodeType::Lesser;
            case Token::Greater:        return NodeType::Greater;
            case Token::LesserEqual:    return NodeType::LesserEqual;
            case Token::GreaterEqual:   return NodeType::GreaterEqual;
            default:                    return nullopt;
        }
    };

    auto unary_operators = [](Token token) -> optional<NodeType>
    {
        switch(token)
        {
            case Token::Increment:  return NodeType::Increment;
            case Token::Decrement:  return NodeType::Decrement;
            default:                return nullopt;
        }
    };

    
    ParserState state = ParserSuccess(nullptr);
    shared_ptr<SyntaxTree> tree, subtree;
    optional<NodeType> opa2;

    /*
	    |   ((unaryOperators identifier | identifier unaryOperators) newLine)
		    ->  ^(unaryOperators identifier)
    */
    if(std::next(begin) != end)
    {
        auto second = std::next(begin);
        if((begin->type == Token::Identifier && (opa2 = unary_operators(second->type)))
        || (second->type == Token::Identifier && (opa2 = unary_operators(begin->type))))
        {
            ParserState state = ParserSuccess(nullptr);
            ParserState ident = ParserSuccess(nullptr);

            auto it = std::next(second);

            if(!expect_newline(state, it, end)) return std::make_pair(end, std::move(state));
            it = parser_aftertoken(it, end, Token::NewLine);

            it = parser_aftertoken(it, end, Token::NewLine);

            auto op_it = (begin->type == Token::Identifier? second : begin);
            auto id_it = (begin->type != Token::Identifier? second : begin);

            std::tie(std::ignore, ident) = parse_identifier(parser, id_it, end);

            shared_ptr<SyntaxTree> tree(new SyntaxTree(opa2.value(), parser.instream, *op_it));
            tree->add_child(get<ParserSuccess>(ident).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
    }

    /*
        |	(id=identifier opa2=assignmentOperators2 a=argument newLine)
        ->  ^(OP_EQ $id ^($opa2  ^($id) $a))
    */
    if(begin->type == Token::Identifier
        && std::next(begin) != end
        && (opa2 = assignment_operators2(std::next(begin)->type)))
    {
        ParserState lhs, rhs;

        std::tie(std::ignore, lhs) = parse_identifier(parser, begin, end);

        auto it = std::next(begin, 2);
        if(it == end)
        {
            return std::make_pair(end, make_error(ParserError(ParserStatus::Error, begin, "XXX unexpected end of expression")));
        }

        std::tie(it, rhs) = parse_argument(parser, it, end);
        add_error(state, rhs);

        expect_newline(state, it, end);
        it = parser_aftertoken(it, end, Token::NewLine);

        if(is<ParserSuccess>(state))
        {
            tree.reset(new SyntaxTree(NodeType::Equal));
            subtree.reset(new SyntaxTree(opa2.value(), parser.instream, *std::next(begin)));

            tree->add_child(get<ParserSuccess>(lhs).tree);
            subtree->add_child(get<ParserSuccess>(lhs).tree->clone());
            subtree->add_child(get<ParserSuccess>(rhs).tree);
            tree->add_child(std::move(subtree));

            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, std::move(state));
        }
    }

    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

// (WS* IDENTIFIER argument* newLine) -> ^(COMMAND IDENTIFIER argument*)
static ParserResult parse_actual_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::Command)
    {
        auto it = std::next(begin);
        ParserState arguments;

        std::tie(it, arguments) = parse_until_if(parse_argument, parser, it, end, [](token_iterator t) {
            return t->type == Token::NewLine;
        });

        if(it != end)
        {
            Expects(it->type == Token::NewLine);
            ++it;
        }

        if(is<ParserSuccess>(arguments))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Command, parser.instream, *begin));
            tree->add_child(shared_ptr<SyntaxTree> { new SyntaxTree(NodeType::Identifier, parser.instream, *begin) });
            tree->take_childs(get<ParserSuccess>(arguments).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, giveup_to_expected(std::move(arguments), "argument"));
        }
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

/*
    positiveCommandStatement
	    :	expressionStatement
	    |	(WS* IDENTIFIER argument* newLine) -> ^(COMMAND IDENTIFIER argument*)
	    ;
*/
static ParserResult parse_positive_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    auto result = parse_oneof(parser, begin, end,
                              parse_expression_statement,
                              parse_actual_command_statement);
    if(is<ParserSuccess>(result.second))
        return result;
    else
    {
        result.first = parser_aftertoken(begin, end, Token::NewLine);
        return result;
    }
}

/*
    commandStatement
	    :	WS* NOT positiveCommandStatement -> ^(NOT positiveCommandStatement)
	    |	positiveCommandStatement -> positiveCommandStatement
	    ;
*/
static ParserResult parse_command_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::NOT)
    {
        ParserState state = ParserSuccess(nullptr);
        ParserState positive_command = ParserSuccess(nullptr);

        auto it = std::next(begin);
        if(it == end || it->type == Token::NewLine)
            add_error(state, ParserError(ParserStatus::Error, begin, "XXX expected command after NOT"));

        std::tie(it, positive_command) = parse_positive_command_statement(parser, it, end);
        add_error(state, positive_command);

        if(is<ParserSuccess>(state))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::NOT, parser.instream, *begin));
            tree->add_child(get<ParserSuccess>(positive_command).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, std::move(state));
        }
    }
    else 
    {
        return parse_positive_command_statement(parser, begin, end);
    }
}

/*
    keyStatement
	    :	WS* MISSION_START newLine -> MISSION_START
	    |	WS* MISSION_END newLine -> MISSION_END
	    |	WS* SCRIPT_START newLine -> SCRIPT_START
	    |	WS* SCRIPT_END newLine -> SCRIPT_END
	    ;

    loopControlStatement
    :	WS* (BREAK -> BREAK | CONTINUE -> CONTINUE) newLine
    ;
*/
static ParserResult parse_keycommand_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    NodeType type = begin->type == Token::MISSION_START? NodeType::MISSION_START :
                    begin->type == Token::MISSION_END?   NodeType::MISSION_END :
                    begin->type == Token::SCRIPT_START?  NodeType::SCRIPT_START :
                    begin->type == Token::SCRIPT_END?    NodeType::SCRIPT_END :
                    begin->type == Token::BREAK?         NodeType::BREAK :
                    begin->type == Token::CONTINUE?      NodeType::CONTINUE :
                                                         NodeType::Ignore;
    if(type != NodeType::Ignore)
    {
        ParserState state = ParserSuccess(nullptr);

        auto it = std::next(begin);

        if(!expect_newline(state, it, end))
        {
            it = parser_aftertoken(it, end, Token::NewLine);
            return std::make_pair(it, std::move(state));
        }

        return std::make_pair(std::next(it), ParserSuccess(new SyntaxTree(type, parser.instream, *begin)));
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

/*
    labelStatement
	    :	WS* IDENTIFIER ':'
	    ->	^(LABEL IDENTIFIER)
	    ;
*/
static ParserResult parse_label_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    // TODO -> ^(LABEL)?
    if(begin->type == Token::Label)
    {
        shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::Label, parser.instream, *begin));
        TokenData temp__{ begin->type, begin->begin, begin->end - 1 }; // TODO 
        tree->add_child(shared_ptr<SyntaxTree> { new SyntaxTree(NodeType::Identifier, parser.instream, temp__) });
        return std::make_pair(std::next(begin), ParserSuccess(std::move(tree)));
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

/*
    variableDeclaration
	    :	WS*
		    type=(VAR_INT|LVAR_INT|VAR_FLOAT|LVAR_FLOAT|VAR_TEXT_LABEL|LVAR_TEXT_LABEL|VAR_TEXT_LABEL16|LVAR_TEXT_LABEL16)
		    variableDeclarationIdentity+
		    newLine
	    -> ^($type variableDeclarationIdentity+)
	    ;
	
    variableDeclarationIdentity
	    :	WS+ IDENTIFIER ('['  integerConstant WS* ']')?
	    ->  ^(IDENTIFIER integerConstant?)
	    ;
*/
static ParserResult parse_variable_declaration(ParserContext& parser, token_iterator begin, token_iterator end)
{
    NodeType type = begin->type == Token::VAR_INT? NodeType::VAR_INT :
                    begin->type == Token::LVAR_INT? NodeType::LVAR_INT :
                    begin->type == Token::VAR_FLOAT? NodeType::VAR_FLOAT :
                    begin->type == Token::LVAR_FLOAT? NodeType::LVAR_FLOAT :
                    begin->type == Token::VAR_TEXT_LABEL? NodeType::VAR_TEXT_LABEL :
                    begin->type == Token::LVAR_TEXT_LABEL? NodeType::LVAR_TEXT_LABEL :
                    begin->type == Token::VAR_TEXT_LABEL16? NodeType::VAR_TEXT_LABEL16 :
                    begin->type == Token::LVAR_TEXT_LABEL16? NodeType::LVAR_TEXT_LABEL16 :
                                                               NodeType::Ignore;

    if(type != NodeType::Ignore)
    {
        auto it = std::next(begin);
        ParserState idents;

        std::tie(it, idents) = parse_until_if(parse_identifier, parser, it, end, [](token_iterator t) {
            return t->type == Token::NewLine;
        });

        if(it != end)
        {
            Expects(it->type == Token::NewLine);
            ++it;
        }

        if(is<ParserSuccess>(idents))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(type, parser.instream, *begin));
            tree->take_childs(get<ParserSuccess>(idents).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, giveup_to_expected(std::move(idents), "identifier"));
        }
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
}

/*
    conditionList
	    :	(conditionListSingle|conditionListAnd|conditionListOr)
	    ;
	
    conditionListSingle
	    :	commandStatement
	    ->  ^(commandStatement)
	    ;
	
    conditionListAnd
	    :	commandStatement ((WS*)! AND commandStatement)+
	    ->	^(AND commandStatement+)
	    ;
	
    conditionListOr
	    :	commandStatement ((WS*)! OR commandStatement)+
	    ->	^(OR commandStatement+)
	    ;
*/
static ParserResult parse_condition_list(ParserContext& parser, token_iterator begin, token_iterator end)
{
    ParserState state = ParserSuccess(nullptr);
    ParserState command;

    auto it = begin;

    std::vector<shared_ptr<SyntaxTree>> conditions;

    enum class CondType
    {
        Single, And, Or,

    } cond_type = CondType::Single;

    for(size_t i = 0; it != end; ++i)
    {
        if(i > 0)
        {
            if(it->type == Token::AND)
            {
                if(cond_type == CondType::Single)
                    cond_type = CondType::And;
                else if(cond_type != CondType::And)
                    add_error(state, ParserError(ParserStatus::Error, it, "XXX expected OR, got AND"));
                ++it;
            }
            else if(it->type == Token::OR)
            {
                if(cond_type == CondType::Single)
                    cond_type = CondType::Or;
                else if(cond_type != CondType::Or)
                    add_error(state, ParserError(ParserStatus::Error, it, "XXX expected AND, got OR"));
                ++it;
            }
            else
            {
                break;
            }
        }

        std::tie(it, command) = parse_command_statement(parser, it, end);
        add_error(state, command);
        if(is<ParserSuccess>(command))
        {
            conditions.emplace_back(get<ParserSuccess>(command).tree);
        }
    }

    if(is<ParserSuccess>(state))
    {
        assert(conditions.size() > 0);

        if(conditions.size() == 1)
        {
            assert(cond_type == CondType::Single);
            return std::make_pair(it, ParserSuccess(conditions.front()));
        }
        else // > 0
        {
            assert(cond_type == CondType::And || cond_type == CondType::Or);
            shared_ptr<SyntaxTree> tree(new SyntaxTree(cond_type == CondType::And? NodeType::AND : NodeType::OR));
            for(auto& cond : conditions) tree->add_child(cond);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
    }
    else
    {
        return std::make_pair(it, std::move(state));
    }
}


/*
    whileStatement
	    :	WS* WHILE conditionList
			    statementList
		    WS* ENDWHILE newLine
	    ->  ^(WHILE conditionList statementList)
	    ;
*/
static ParserResult parse_while_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::WHILE)
    {
        ParserState state = ParserSuccess(nullptr);
        ParserState conditions, statements;

        auto it = std::next(begin);
        if(it == end)
        {
            add_error(state, make_error(ParserError(ParserStatus::Error, begin, "XXX missing ENDWHILE for this")));
            return std::make_pair(it, state);
        }

        std::tie(it, conditions) = parse_condition_list(parser, it, end);
        add_error(state, giveup_to_expected(conditions, "command"));

        std::tie(it, statements) = parse_until_if(parse_statement, parser, it, end, [](token_iterator t) {
            return t->type == Token::ENDWHILE;
        });
        add_error(state, giveup_to_expected(statements, "statement"));

        if(it == end)
        {
            add_error(state, make_error(ParserError(ParserStatus::Error, begin, "XXX missing ENDWHILE for this")));
            return std::make_pair(it, state);
        }

        Expects(it->type == Token::ENDWHILE);
        ++it;

        expect_newline(state, it, end);
        it = parser_aftertoken(it, end, Token::NewLine);

        if(is<ParserSuccess>(state))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::WHILE, parser.instream, *begin));
            tree->add_child(get<ParserSuccess>(conditions).tree);
            tree->add_child(get<ParserSuccess>(statements).tree);
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, std::move(state));
        }
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
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
static ParserResult parse_if_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    if(begin->type == Token::IF)
    {
        ParserState state = ParserSuccess(nullptr);
        ParserState           conditions;
        ParserState           body_true;
        optional<ParserState> body_false;
        token_iterator        it_else;

        auto it = std::next(begin);
        if(it == end)
        {
            add_error(state, make_error(ParserError(ParserStatus::Error, begin, "XXX missing ENDIF for this")));
            return std::make_pair(it, state);
        }

        std::tie(it, conditions) = parse_condition_list(parser, it, end);
        add_error(state, giveup_to_expected(conditions, "command"));

        std::tie(it, body_true) = parse_until_if(parse_statement, parser, it, end, [](token_iterator t) {
            return t->type == Token::ELSE || t->type == Token::ENDIF;
        });
        add_error(state, giveup_to_expected(body_true, "statement"));

        if(it == end)
        {
            add_error(state, make_error(ParserError(ParserStatus::Error, begin, "XXX missing ENDIF for this")));
            return std::make_pair(it, state);
        }

        Expects(it->type == Token::ELSE || it->type == Token::ENDIF);

        if(it->type == Token::ELSE)
        {
            it_else = it;

            ParserState body_false1;

            ++it;

            expect_newline(state, it, end);
            it = parser_aftertoken(it, end, Token::NewLine);

            std::tie(it, body_false1) = parse_until_if(parse_statement, parser, it, end, [](token_iterator t) {
                return t->type == Token::ENDIF;
            });
            add_error(state, giveup_to_expected(body_false1, "statement"));

            if(it == end)
            {
                add_error(state, make_error(ParserError(ParserStatus::Error, begin, "XXX missing ENDIF for this")));
                return std::make_pair(it, state);
            }

            body_false.emplace(std::move(body_false1));
        }

        Expects(it->type == Token::ENDIF);
        ++it;

        expect_newline(state, it, end);
        it = parser_aftertoken(it, end, Token::NewLine);

        if(is<ParserSuccess>(state))
        {
            shared_ptr<SyntaxTree> tree(new SyntaxTree(NodeType::IF, parser.instream, *begin));

            tree->add_child(get<ParserSuccess>(conditions).tree);
            tree->add_child(get<ParserSuccess>(body_true).tree);

            if(body_false)
            {
                shared_ptr<SyntaxTree> else_tree(new SyntaxTree(NodeType::ELSE, parser.instream, *it_else));
                else_tree->add_child(get<ParserSuccess>(*body_false).tree);
                tree->add_child(std::move(else_tree));
            }
            
            return std::make_pair(it, ParserSuccess(std::move(tree)));
        }
        else
        {
            return std::make_pair(it, std::move(state));
        }
    }
    return std::make_pair(end, make_error(ParserStatus::GiveUp));
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
static ParserResult parse_statement(ParserContext& parser, token_iterator begin, token_iterator end)
{
    auto result = parse_oneof(parser, begin, end,
                              parse_newline,
                              parse_scope_statement,
                              parse_if_statement,
                              parse_while_statement,
                              // parse_repeat_statement, TODO
                              // parse_switch_statement, TODO
                              parse_variable_declaration,
                              parse_label_statement,
                              parse_keycommand_statement,
                              parse_command_statement);

    if(parser_isgiveup(result.second))
    {
        auto it = parser_aftertoken(begin, end, Token::NewLine);
        return std::make_pair(it, std::move(result.second));
    }
    return result;
}

static ParserResult parser(ParserContext& parser, token_iterator begin, token_iterator end)
{
    auto result = parse_until_if(parse_statement, parser, begin, end, [](token_iterator) { return false; });
    if(is<ParserSuccess>(result.second))
        return result;
    else
        return std::make_pair(result.first, giveup_to_expected(std::move(result.second), "statement"));
}


//
// SyntaxTree
//

SyntaxTree::SyntaxTree(SyntaxTree&& rhs)
    : type_(rhs.type_), token(std::move(rhs.token)), childs(std::move(rhs.childs)), parent_(std::move(rhs.parent_)),
      udata(std::move(rhs.udata)), instream(std::move(rhs.instream))
{
    rhs.type_ = NodeType::Ignore;
    rhs.parent_ = nullopt;
}

shared_ptr<SyntaxTree> SyntaxTree::clone()
{
    shared_ptr<SyntaxTree> tree(new SyntaxTree(this->type_));
    tree->token = this->token;
    tree->instream = this->instream;
    tree->udata = this->udata;
    
    for(auto& child : this->childs)
        tree->add_child(child->clone());

    return tree;
}

std::shared_ptr<SyntaxTree> SyntaxTree::compile(ProgramContext& program, const TokenStream& tstream)
{
    ParserContext parser_ctx(program, tstream);

    ParserResult presult = parser(parser_ctx, tstream.tokens.data(), tstream.tokens.data() + tstream.tokens.size());

    if(is<ParserSuccess>(presult.second))
    {
        if(true)
        {
            auto& k = get<ParserSuccess>(presult.second);
            puts(get<ParserSuccess>(presult.second).tree->to_string().c_str());
            puts("---------------------------\n");
            //throw 0; // TODO remove me
            return get<ParserSuccess>(presult.second).tree;
        }
        throw 2; // TODO remove me
    }
    else
    {
        for(auto& error : get<ParserFailure>(presult.second))
        {
            if(error.state == ParserStatus::GiveUp)
                continue;

            if(error.it_stop == tstream.tokens.data() + tstream.tokens.size()) // == end
            {
                program.error(nocontext, error.error.c_str());
            }
            else 
            {
                TokenStream::TokenInfo token_info{ tstream.shared_from_this(), *error.it_stop };
                program.error(token_info, error.error.c_str());
            }
        }
        throw 1; // TODO remove me
        return nullptr;
    }
}

std::string SyntaxTree::to_string(size_t level) const
{
    std::string output = fmt::format("{:>{}} ({}) {}\n", ' ', level * 4, int(this->type()),
                                                              this->has_text()? this->text() : "");

    for(auto& child : *this)
    {
        output += child->to_string(level + 1);
    }

    return output;
}
