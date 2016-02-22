#pragma once
#include "cxx17/filesystem.hpp"
#include <string>
#include <vector>

// forward ANTLR structs
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
    SyntaxTree(const SyntaxTree&) = delete;            // expensive, make a explicit method if needed
    SyntaxTree(SyntaxTree&&);
    SyntaxTree& operator=(const SyntaxTree&) = delete; // expensive, make a explicit method if needed
    SyntaxTree& operator=(SyntaxTree&&);
    
    static SyntaxTree compile(const TokenStream& tstream);

    static SyntaxTree from_raw_tree(ANTLR3_BASE_TREE_struct*);

    // contains state changes
    const std::string& text() const
    {
        return this->data;
    }

    size_t SyntaxTree::child_count() const
    {
        return this->childs.size();
    }
    
    const SyntaxTree& SyntaxTree::child(size_t i) const
    {
        return this->childs[i];
    }

    uint32_t type() const
    {
        return this->type_;
    }

    std::string to_string() const;

    // left to right, including visiting childs of left before going to the right
    // bool(SyntaxTree)
    template<typename Functor>
    void walk(Functor fun) const
    {
        auto fun_ref = std::ref(fun);
        for(size_t i = 0, max = this->child_count(); i < max; ++i)
        {
            auto& child = this->child(i);
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
    int                     type_;
    std::string             data;
    std::vector<SyntaxTree> childs;

    explicit SyntaxTree(int type, std::string data = std::string())
        : type_(type), data(std::move(data))
    {
    }
};
