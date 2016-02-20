#pragma once
#include "cxx17/filesystem.hpp"
#include "cxx17/optional.hpp"
#include <string>

struct gta3scriptLexer_Ctx_struct;
struct gta3scriptParser_Ctx_struct;
struct ANTLR3_COMMON_TOKEN_STREAM_struct;
struct ANTLR3_COMMON_TOKEN_struct;
struct ANTLR3_INPUT_STREAM_struct;
struct ANTLR3_BASE_TREE_struct;

class TokenStream
{
public:
    explicit TokenStream(const fs::path& path);
    explicit TokenStream(std::string data, const char* stream_name);
    ~TokenStream();

    TokenStream(const TokenStream&) = delete;
    TokenStream(TokenStream&&);
    TokenStream& operator=(const TokenStream&) = delete;
    TokenStream& operator=(TokenStream&&);

protected:
    friend class SyntaxTree;

    ANTLR3_COMMON_TOKEN_STREAM_struct* get() const {
        return tokstream;
    }

private:
    std::string data;
    ANTLR3_INPUT_STREAM_struct* istream;
    gta3scriptLexer_Ctx_struct* lexer;
    ANTLR3_COMMON_TOKEN_STREAM_struct* tokstream;
};

class SyntaxTree
{
public:
    SyntaxTree(const SyntaxTree&);             // copy is only be allowed when this->parser == nullptr
    SyntaxTree(SyntaxTree&&);
    SyntaxTree& operator=(const SyntaxTree&);  // copy is only be allowed when this->parser == nullptr
    SyntaxTree& operator=(SyntaxTree&&);
    ~SyntaxTree();

    static SyntaxTree compile(const TokenStream& tstream);


    // contains state changes
    const std::string& text() const;

    size_t SyntaxTree::child_count() const;
    SyntaxTree SyntaxTree::child(size_t i) const;
    uint32_t type() const;


    ANTLR3_BASE_TREE_struct* get() const {
        return tree;
    }

    std::string to_string() const;

    SyntaxTree shallow_copy() const
    {
        return SyntaxTree(nullptr, this->tree);
    }

    // left to right, including visiting childs of left before going to the right
    // bool(SyntaxTree)
    template<typename Functor>
    void walk(Functor fun) const
    {
        auto fun_ref = std::ref(fun);
        for(size_t i = 0, max = this->child_count(); i < max; ++i)
        {
            auto child = this->child(i);
            if(fun(child)) child.walk(fun_ref);
        }
    }

    // left to right, no childs
    // void(SyntaxTree)
    template<typename Functor>
    void walk_top(Functor fun) const
    {
        auto fun_ref = std::ref(fun);
        for(size_t i = 0, max = this->child_count(); i < max; ++i)
        {
            fun(this->child(i));
        }
    }

private:
    ANTLR3_BASE_TREE_struct* tree;
    gta3scriptParser_Ctx_struct* parser;
    optional<std::string> cached_text;

    explicit SyntaxTree(gta3scriptParser_Ctx_struct* parser, ANTLR3_BASE_TREE_struct* tree);
};

class Token
{
public:
    Token(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(const Token&) = default;
    Token& operator=(Token&&) = default;

protected:
    explicit Token(ANTLR3_COMMON_TOKEN_struct* token);

private:
    ANTLR3_COMMON_TOKEN_struct* token;
};
