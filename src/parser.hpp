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

class TokenStream : public std::enable_shared_from_this<TokenStream>
{
public:
    explicit TokenStream(const fs::path& path);
    explicit TokenStream(std::string data, const char* stream_name);
    ~TokenStream();

    TokenStream(const TokenStream&) = delete;
    TokenStream(TokenStream&&);
    TokenStream& operator=(const TokenStream&) = delete;
    TokenStream& operator=(TokenStream&&);

    const std::string& name() const
    {
        return this->sname;
    }

    const std::string& text() const
    {
        return this->data;
    }

    /// Gets the byte offset in this->text() that the specified line number (1-based) is in.
    ///
    /// \throws std::logic_error if lineno does not exist.
    size_t offset_for_line(size_t lineno) const
    {
        size_t i = (lineno - 1);
        if(i < line_offset.size())
            return line_offset[i];
        else
            throw std::logic_error("Bad `lineno` in TokenStream::offset_for_line");
    }

    /// (1-based)
    ///
    /// \throws std::logic_error if lineno does not exist.
    std::string get_line(size_t lineno) const;


protected:
    friend class SyntaxTree;

    ANTLR3_COMMON_TOKEN_STREAM_struct* get() const {
        return tokstream;
    }

private:
    // My stuff.
    std::string             sname;
    std::string             data;
    std::vector<size_t>     line_offset;

    // ANTLR stuff.
    ANTLR3_INPUT_STREAM_struct* istream;
    gta3scriptLexer_Ctx_struct* lexer;
    ANTLR3_COMMON_TOKEN_STREAM_struct* tokstream;

    explicit TokenStream(optional<std::string> data, const char* stream_name);

    void calc_lines();
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

    std::string filename() const
    {
        return this->instream? *this->instream->filename : "";
    }

    weak_ptr<const TokenStream> token_stream() const
    {
        return this->instream? this->instream->tstream : weak_ptr<const TokenStream>();
    }

    uint32_t line() const
    {
        return this->lineno;
    }

    uint32_t column() const
    {
        return this->colno;
    }

    // contains state changes
    const std::string& text() const
    {
        return this->data;
    }

    bool has_text() const
    {
        return !this->text().empty();
    }

    size_t child_count() const
    {
        return this->childs.size();
    }
    
    const SyntaxTree& child(size_t i) const
    {
        return *this->childs[i];
    }

    SyntaxTree& child(size_t i)
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

protected:
    friend class TokenStream;

    struct InputStream
    {
        shared_ptr<std::string>     filename;   //< Name of the input file. Stored also here because tstream may get deallocated.
        weak_ptr<const TokenStream> tstream;    //< Input token stream, if still allocated.
    };

    static std::shared_ptr<SyntaxTree> from_raw_tree(ANTLR3_BASE_TREE_struct*,
                                                     const shared_ptr<InputStream>& instream = nullptr);

private:

    // This data structure assumes all those members are constant for the entire lifetime of this object!
    // ...except for udata
    NodeType                                    type_;
    std::string                                 data;
    std::vector<std::shared_ptr<SyntaxTree>>    childs;
    optional<std::weak_ptr<SyntaxTree>>         parent_;
    any                                         udata;
    shared_ptr<InputStream>                     instream;   //<
    uint32_t                                    lineno;     //< Line number (starting from 1)
    uint32_t                                    colno;      //< Column number (starting from 1)
    

    explicit SyntaxTree(int type, std::string data)
        : type_(NodeType(type)), data(std::move(data))
    {
    }
};
