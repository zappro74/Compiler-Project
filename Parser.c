#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Lexer.h"

typedef struct
{
    Lexer *lexer;
    Token *token;
} Parser;

//ASTNode/Node struct... Doing research on this

Parser* init_parser(Lexer *lexer) 
{
    Parser *parser = (Parser*)malloc(sizeof(Parser)); //allocate memory for the Parser
    if (!parser)
    {
        fprintf(stderr, "memory allocation failed\n");
        exit(1);
    }

    parser -> lexer = lexer;
    parser -> token = next_token(lexer);

    return parser;
}

void advance(Parser *parser)
{
   
}

int match(Parser *parser, TokenType expected_type)
{

}

void expect(Parser *parser, TokenType expected_type)
{

}

