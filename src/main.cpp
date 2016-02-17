#include <string>

#include "grammar/autogen/gta3scriptLexer.h"
#include "grammar/autogen/gta3scriptParser.h"
#undef true
#undef false
//#undef EOF

///////////////////////////////////////////////////////////////////////////

class SyntaxTree
{
public:
    SyntaxTree(pANTLR3_BASE_TREE tree, bool to_owned);
    ~SyntaxTree();

    static SyntaxTree compile(const std::string&

private:
    bool owner;
    pANTLR3_BASE_TREE tree;
};

class Token
{
public:
    Token(pANTLR3_COMMON_TOKEN tree, bool to_owned);
    ~Token();

private:
    bool owner;
    pANTLR3_COMMON_TOKEN token;
};

///////////////////////////////////////////////////////////////////////////

SyntaxTree::SyntaxTree(pANTLR3_BASE_TREE tree, bool to_owned)
    : owner(to_owned), tree(tree)
{
}

SyntaxTree::~SyntaxTree()
{
    if(owner) tree->free(tree);
}

Token::Token(pANTLR3_COMMON_TOKEN token, bool to_owned)
    : owner(to_owned), token(token)
{
}

Token::~Token()
{
    if(owner) token->free(token);
}

///////////////////////////////////////////////////////////////////////////

int main()
{
    //antlr3FileStreamNew(filename, ANTLR3_ENC_UTF8);

    std::string str = "TEST";

    auto istream = antlr3StringStreamNew((pANTLR3_UINT8)(str.data()), ANTLR3_ENC_UTF8, str.size(), (pANTLR3_UINT8)"name");
    auto lexer = gta3scriptLexerNew(istream);
    auto tokstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    auto parser = gta3scriptParserNew(tokstream);

    auto stree = parser->start(parser);

    auto x = stree.tree->toStringTree(stree.tree);
    printf("%s\n", x->chars);
    
    // TODO free the stream and lexer and aalal

    getchar();
}

