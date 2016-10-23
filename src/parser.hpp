#pragma once
#include "stdinc.h"

struct ParserContext;

// TODO REMOVE
#include "grammar/autogen/gta3scriptsTokens.hpp"
using NodeType = Token;

class TokenStream : public std::enable_shared_from_this<TokenStream>
{
public:
    struct TokenData
    {
        Token  type;    //< Type of token
        size_t begin;   //< Offset for token in TokenStream::data
        size_t end;     //< Offset for token in TokenStream::data (end)
    };

public:
    const std::string             stream_name;  //< Name of this stream (usually name of the source file).
    const std::string             data;         //< UTF-8 source file.
    const std::vector<TokenData>  tokens;       //< Tokenized source file.

public:
    static std::shared_ptr<TokenStream> tokenize(ProgramContext&, const fs::path&);
    static std::shared_ptr<TokenStream> tokenize(ProgramContext&, std::string data, const char* stream_name);
    TokenStream(TokenStream&&);
    TokenStream(const TokenStream&) = delete;

    /// Gets the byte offset in this->text() that the specified line number (1-based) is in.
    ///
    /// \throws std::logic_error if lineno does not exist.
    size_t offset_for_line(size_t lineno) const;

    /// Gets the content of the specified line number (1-based).
    ///
    /// \throws std::logic_error if lineno does not exist.
    std::string get_line(size_t lineno) const;

    /// Gets the (lineno, colno) (1-based) of the specified offset in the stream data.
    ///
    /// \throws std::logic_error if offset is out of range.
    std::pair<size_t, size_t> linecol_from_offset(size_t offset) const;

    /// For debugging purposes.
    std::string to_string() const;

private:
    ProgramContext&         program;
    std::vector<size_t>     line_offset;

    explicit TokenStream(ProgramContext&, optional<std::string> data, const char* stream_name);
    explicit TokenStream(ProgramContext&, const char* stream_name, std::string data, std::vector<TokenData>);

    void calc_lines();
};

class SyntaxTree : public std::enable_shared_from_this<SyntaxTree>
{
public:
    using const_iterator = std::vector<std::shared_ptr<SyntaxTree>>::const_iterator;

public:
    static std::shared_ptr<SyntaxTree> compile(ProgramContext&, const TokenStream& tstream);
    SyntaxTree(const SyntaxTree&) = delete;
    SyntaxTree(SyntaxTree&&);
    
    /// Builds a temporary SyntaxTree which isn't dynamically allocated (for shared_ptr).
    ///
    /// \warning Be careful, `shared_from_this()` returns null for these.
    template<typename T>
    static SyntaxTree temporary(NodeType type, T udatax)
    {
        return SyntaxTree(type, any(std::move(udatax)));
    }

    /// Gets the type of this node.
    NodeType type() const
    {
        return this->type_;
    }

    /// Text associated with the token in this node.
    string_view text() const
    {
        auto source_data = this->instream->tstream.lock()->data.c_str();
        return string_view(source_data + this->token.begin, this->token.end - this->token.begin);
    }

    /// Checks if `text().empty()`.
    bool has_text() const
    {
        return this->token.begin != this->token.end;
    }

    /// Iterator to childs (begin).
    const_iterator begin() const
    {
        return this->childs.begin();
    }

    /// Iterator to childs (end).
    const_iterator end() const
    {
        return this->childs.end();
    }

    /// Number of childs on this node.
    size_t child_count() const
    {
        return this->childs.size();
    }
    
    /// Gets the child at the specified index.
    const SyntaxTree& child(size_t i) const
    {
        return *this->childs[i];
    }

    /// Gets the child at the specified index.
    SyntaxTree& child(size_t i)
    {
        return *this->childs[i];
    }

    /// Gets the parent node, or `nullptr` if none.
    std::shared_ptr<SyntaxTree> parent() const
    {
        if(this->parent_)
            return this->parent_.value().lock();
        return nullptr;
    }

    // Adds a child to this node.
    void add_child(shared_ptr<SyntaxTree> child)
    {
        child->parent_ = std::weak_ptr<SyntaxTree>(this->shared_from_this());
        this->childs.emplace_back(std::move(child));
    }

    /// Performs a pre-ordered depth-first traversal on this tree.
    ///
    /// Does not go any deeper in a node that `fun()` returns false.
    template<typename Functor>  // Functor = bool(SyntaxTree)
    void depth_first(Functor fun) //const
    {
        if(fun(*this))
        {
            for(size_t i = 0, max = this->child_count(); i < max; ++i)
            {
                this->child(i).depth_first(std::ref(fun));
            }
        }
    }

    /// Sets the annotation for this node.
    template<typename ValueType>
    void set_annotation(ValueType&& v)
    {
        this->udata = std::forward<ValueType>(v);
    }

    /// Gets the annotation of this node, previosly set with `set_annotation`.
    ///
    /// Note: You can get a ref by using e.g. `<int&>` instead of `<int>`.
    ///
    /// \throws bad_any_cast if there's no annotation on this node.
    template<typename T> // 
    T annotation() const
    {
        return any_cast<T>(this->udata);
    }

    /// Gets the annotation of this node, previosly set with `set_annotation`, or `nullopt` if not set.
    ///
    /// Note: You can get a ref by using e.g. `<int&>` instead of `<int>`.
    template<typename T>
    optional<T> maybe_annotation() const
    {
        using TNoRef = std::remove_reference_t<T>;
        if(const TNoRef* p = any_cast<TNoRef>(&this->udata))
            return *p;
        return nullopt;
    }

    /// Checks if this node has been annotated.
    bool is_annotated() const
    {
        return !this->udata.empty();
    }

    /// Filename of the input stream associated with this SyntaxTree, or empty if none.
    std::string filename() const
    {
        return this->instream? *this->instream->filename : "";
    }

    /// Input stream associated with this SyntaxTree, or `nullptr` if none.
    weak_ptr<const TokenStream> token_stream() const
    {
        return this->instream? this->instream->tstream : weak_ptr<const TokenStream>();
    }

    /// Offset to this token in the input stream data.
    size_t offset() const
    {
        return this->token.begin;
    }

    /// For debugging purposes.
    std::string to_string(size_t level = 0) const;


protected:
    friend class TokenStream;
    friend struct ParserContext;

    struct InputStream
    {
        shared_ptr<std::string>     filename;   //< Name of the input file. Stored also here because tstream may get deallocated.
        weak_ptr<const TokenStream> tstream;    //< Input token stream, if still allocated.
    };

private:
    NodeType                                    type_;  // const NodeType
    TokenStream::TokenData                      token;
    std::vector<std::shared_ptr<SyntaxTree>>    childs;
    optional<std::weak_ptr<SyntaxTree>>         parent_;
    any                                         udata;
    shared_ptr<InputStream>                     instream;

public:
    explicit SyntaxTree(NodeType type, any udata)
        : type_(type), udata(udata), instream(nullptr)
    {
        // this->token will be trash, fix it? TODO
    }

    explicit SyntaxTree(NodeType type, shared_ptr<InputStream>& instream, const TokenStream::TokenData& token)
        : type_(type), instream(instream), token(token)
    {
    }
};
