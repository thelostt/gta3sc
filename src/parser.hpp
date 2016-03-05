#pragma once
#include "stdinc.h"

// forward ANTLR structs
struct gta3scriptLexer_Ctx_struct;
struct gta3scriptParser_Ctx_struct;
struct ANTLR3_COMMON_TOKEN_STREAM_struct;
struct ANTLR3_COMMON_TOKEN_struct;
struct ANTLR3_INPUT_STREAM_struct;
struct ANTLR3_BASE_TREE_struct;

#include "grammar/autogen/gta3scriptsTokens.hpp"
using NodeType = Token;

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

class SyntaxTree : public std::enable_shared_from_this<SyntaxTree>
{
public:
    using const_iterator = std::vector<std::shared_ptr<SyntaxTree>>::const_iterator;

    SyntaxTree(const SyntaxTree&) = delete;            // expensive, make a explicit method if needed
    SyntaxTree(SyntaxTree&&);
    SyntaxTree& operator=(const SyntaxTree&) = delete; // expensive, make a explicit method if needed
    SyntaxTree& operator=(SyntaxTree&&);
    
    static std::shared_ptr<SyntaxTree> compile(const TokenStream& tstream);

    static std::shared_ptr<SyntaxTree> from_raw_tree(ANTLR3_BASE_TREE_struct*);

    const_iterator begin() const
    {
        return this->childs.begin();
    }

    const_iterator end() const
    {
        return this->childs.end();
    }

    const_iterator find(const SyntaxTree& child) const
    {
        return std::find_if(begin(), end(), [&](const std::shared_ptr<SyntaxTree>& node) {
            return node.get() == std::addressof(child);
        });
    }

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
        return *this->childs[i];
    }

    SyntaxTree& SyntaxTree::child(size_t i)
    {
        return *this->childs[i];
    }

    NodeType type() const
    {
        return this->type_;
    }

    std::shared_ptr<SyntaxTree> parent() const
    {
        if(this->parent_)
            return this->parent_.value().lock();
        return nullptr;
    }


    std::string to_string() const;

    // left to right, including visiting childs of left before going to the right
    // does **not** visit myself
    // bool(SyntaxTree)
    template<typename Functor>
    void walk(Functor fun) //const
    {
        auto fun_ref = std::ref(fun);
        for(size_t i = 0, max = this->child_count(); i < max; ++i)
        {
            auto& child = this->child(i);
            if(fun(child)) child.walk(fun_ref);
        }
    }

    // left to right, including visiting childs of left before going to the right
    // does not visit myself
    // bool(SyntaxTree)
    template<typename Functor>
    void walk_inclusive(Functor fun) //const
    {
        fun(*this);
        this->walk(std::ref(fun));
    }

    // left to right, no childs
    // does **not** visit myself
    // void(SyntaxTree)
    template<typename Functor>
    void walk_top(Functor fun) //const
    {
        auto fun_ref = std::ref(fun);
        for(size_t i = 0, max = this->child_count(); i < max; ++i)
        {
            fun(this->child(i));
        }
    }

    template<typename ValueType>
    void set_annotation(ValueType&& v)
    {
        this->udata = std::forward<ValueType>(v);
    }

    template<typename T> // you can get a ref by using e.g. <int&> instead of <int>
    T annotation() const
    {
        return any_cast<T>(this->udata);
    }

    template<typename T> // you can get a ref by using e.g. <int&> instead of <int>
    optional<T> maybe_annotation() const
    {
        using TNoRef = std::remove_reference_t<T>;
        if(const TNoRef* p = any_cast<TNoRef>(&this->udata))
            return *p;
        return nullopt;
    }

    bool is_annotated() const
    {
        return !this->udata.empty();
    }

public:
    /// Builds a temporary SyntaxTree which isn't dynamically allocated (for shared_ptr).
    /// Careful, `shared_from_this()` returns null for these.
    static SyntaxTree temporary(NodeType type, std::string data)
    {
        return SyntaxTree((int)type, std::move(data));
    }

private:
    // This data structure assumes all those members are constant for the entire lifetime of this object!
    // ...except for udata
    NodeType                                    type_;
    std::string                                 data;
    std::vector<std::shared_ptr<SyntaxTree>>    childs;
    optional<std::weak_ptr<SyntaxTree>>         parent_;
    any                                         udata;

    explicit SyntaxTree(int type, std::string data)
        : type_(NodeType(type)), data(std::move(data))
    {
    }
};
