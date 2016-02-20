#include <cassert>
#include "parser.hpp"

// Note: Include those after everything else!
// These headers have some bad defines (including true, false and EOF)
#include "grammar/autogen/gta3scriptLexer.h"
#include "grammar/autogen/gta3scriptParser.h"
#undef true
#undef false




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
// Token
//

Token::Token(pANTLR3_COMMON_TOKEN token)
    : token(token)
{
}

//
// SyntaxTree
//

SyntaxTree::SyntaxTree(pgta3scriptParser parser, pANTLR3_BASE_TREE tree) :
    parser(parser), tree(tree)
{
}

SyntaxTree::SyntaxTree(const SyntaxTree& rhs)
{
    *this = rhs;
}

SyntaxTree::SyntaxTree(SyntaxTree&& rhs)
{
    *this = std::move(rhs);
}

SyntaxTree::~SyntaxTree()
{
    if(this->parser)
        this->parser->free(this->parser);
}

SyntaxTree& SyntaxTree::operator=(SyntaxTree&& rhs)
{
    if(this->parser)
        this->parser->free(this->parser);

    this->parser = rhs.parser;
    this->tree = rhs.tree;
    rhs.parser = nullptr;

    return *this;
}

SyntaxTree& SyntaxTree::operator=(const SyntaxTree& rhs)
{
    assert(rhs.parser == nullptr);

    if(this->parser)
        this->parser->free(this->parser);

    this->parser = rhs.parser;
    this->tree = rhs.tree;

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
            return SyntaxTree(parser, start_tree.tree);
        }
    }

    throw std::runtime_error("TODO error");
}

std::string SyntaxTree::to_string() const
{
    auto raw = tree->toStringTree(tree);
    auto result = std::string((const char*)(raw->chars), raw->len);
    raw->factory->destroy(raw->factory, raw);
    return result;
}

uint32_t SyntaxTree::type() const
{
    return tree->getType(tree);
}

size_t SyntaxTree::child_count() const
{
    return tree->getChildCount(tree);
}

SyntaxTree SyntaxTree::child(size_t i) const
{
    if(void* p = tree->getChild(tree, i))
    {
        return SyntaxTree(nullptr, (pANTLR3_BASE_TREE)(p));
    }
    else
    {
        assert(!"TODO");
    }
}

const std::string& SyntaxTree::text() const
{
    // This function is very stateful despite the constness of the function.
    // Both tree->getText and us will do state changes.

    if(!this->cached_text)
    {
        // VERY STATEFUL BLOCK
        auto str = tree->getText(tree);
        const_cast<SyntaxTree*>(this)->cached_text.emplace((const char*)(str->chars), str->len);
        str->factory->destroy(str->factory, str);
    }
    return this->cached_text.value();
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
