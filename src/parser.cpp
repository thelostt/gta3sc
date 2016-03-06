#include <cassert>
#include "parser.hpp"

// Note: Include those after everything else!
// These headers have some bad defines (including true, false and EOF)
#include "grammar/autogen/gta3scriptLexer.h"
#include "grammar/autogen/gta3scriptParser.h"
#undef true
#undef false
#undef EOF

// TODO std::runtime_error to CompilerError or something


//
// SyntaxTree
//

std::shared_ptr<SyntaxTree> SyntaxTree::from_raw_tree(pANTLR3_BASE_TREE node)
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

        case LONG_STRING:
        case SHORT_STRING:
        {
            data = (const char*) node->getText(node)->chars;
            break;
        }
    }

    std::shared_ptr<SyntaxTree> tree(new SyntaxTree(type, data));
    tree->childs.reserve(child_count);

    for(size_t i = 0; i < child_count; ++i)
    {
        auto node_child = (pANTLR3_BASE_TREE)(node->getChild(node, i));
        if(node_child->getType(node_child) != SKIPS) // newline statements
        {
            auto my_child = from_raw_tree(node_child);
            my_child->parent_ = std::weak_ptr<SyntaxTree>(tree);
            tree->childs.emplace_back(std::move(my_child));
        }
    }

    return tree;
}

SyntaxTree::SyntaxTree(SyntaxTree&& rhs)
    : type_(rhs.type_), data(std::move(rhs.data)), childs(std::move(rhs.childs)), parent_(std::move(rhs.parent_)),
      udata(std::move(rhs.udata))
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
    rhs.type_ = NodeType::Ignore;
    rhs.parent_ = nullopt;
    return *this;
}

std::shared_ptr<SyntaxTree> SyntaxTree::compile(const TokenStream& tstream)
{
    auto parser = gta3scriptParserNew(tstream.get());
    if(parser)
    {
        auto start_tree = parser->start(parser);

        if(start_tree.tree)
        {
            return SyntaxTree::from_raw_tree(start_tree.tree);
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

TokenStream::TokenStream(std::string data_, const char* stream_name)
    : data(std::move(data_))
{
    this->istream = antlr3StringStreamNew((pANTLR3_UINT8)(data.c_str()), ANTLR3_ENC_UTF8, data.size(), (pANTLR3_UINT8)(stream_name));

    if(istream)
    {
        if(this->lexer = gta3scriptLexerNew(istream))
        {
            if(this->tokstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer)))
            {
                return;
            }
        }
    }

    throw std::runtime_error("Failed to open token stream named '" + std::string(stream_name) + "' for reading.");
}

TokenStream::TokenStream(optional<std::string> data_, const char* stream_name)
    : TokenStream((data_? std::move(data_.value()) : throw std::runtime_error("")), stream_name)
{
}

TokenStream::TokenStream(const fs::path& path)
    : TokenStream(read_file_utf8(path), path.generic_u8string().c_str())
{
}

TokenStream::TokenStream(TokenStream&& rhs)
    : tokstream(nullptr), lexer(nullptr), istream(nullptr)
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
    this->data = std::move(rhs.data);

    rhs.tokstream = nullptr;
    rhs.lexer = nullptr;
    rhs.istream = nullptr;

    return *this;
}

TokenStream::~TokenStream()
{
    if(this->tokstream) this->tokstream->free(this->tokstream);
    if(this->lexer) this->lexer->free(this->lexer);
    if(this->istream) this->istream->free(this->istream);
}
