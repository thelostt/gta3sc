#include <cassert>
#include "parser.hpp"

// Note: Include those after everything else!
// These headers have some bad defines (including true, false and EOF)
#include "grammar/autogen/gta3scriptLexer.h"
#include "grammar/autogen/gta3scriptParser.h"
#undef true
#undef false
#undef EOF



std::string read_file_utf8(const fs::path& path)
{
    // TODO move to somewhere more "visible" so it can be useful to somewhere else in the project too
    // TODO this func is terrible ugly
    size_t size;

#ifdef _WIN32
    auto f = _wfopen(path.c_str(), L"rb");
#else
    auto f = fopen(path.c_str(), "rb");
#endif

    if(f != nullptr)
    {
        if(!fseek(f, 0L, SEEK_END)
        && (size = ftell(f)) != -1
        && !fseek(f, 0L, SEEK_SET))
        {
            std::string output;

            output.resize(size);

            if(fread(&output[0], sizeof(char), size, f) == size)
            {
                fclose(f);
                return output;
            }
        }

        fclose(f);
        throw std::runtime_error("TODO");
    }

    throw std::runtime_error("TODO failed to open file");
}

//
// SyntaxTree
//

SyntaxTree SyntaxTree::from_raw_tree(pANTLR3_BASE_TREE node)
{
    std::string data;
    int type = node->getType(node);
    size_t child_count = node->getChildCount(node);

    switch(type)
    {
        case IDENTIFIER:
        case INTEGER:
        case FLOAT:
        {
            data = (const char*) node->getText(node)->chars;
            break;
        }

        case LONG_STRING:
        case SHORT_STRING:
            data = (const char*) node->getText(node)->chars;
            break;
    }

    SyntaxTree tree(type, data);
    tree.childs.reserve(child_count);

    for(size_t i = 0; i < child_count; ++i)
    {
        auto node_child = (pANTLR3_BASE_TREE)(node->getChild(node, i));
        if(node_child->getType(node_child) != SKIPS) // newline statements
        {
            tree.childs.emplace_back(from_raw_tree(node_child));
        }
    }

    return tree;
}

SyntaxTree::SyntaxTree(SyntaxTree&& rhs)
    : type_(rhs.type_), data(std::move(rhs.data)), childs(std::move(rhs.childs))
{
    rhs.type_ = NodeType::Ignore;
}

SyntaxTree& SyntaxTree::operator=(SyntaxTree&& rhs)
{
    this->data = std::move(rhs.data);
    this->childs = std::move(rhs.childs);
    this->type_ = rhs.type_;
    rhs.type_ = NodeType::Ignore;
    return *this;
}

SyntaxTree SyntaxTree::compile(const TokenStream& tstream)
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
