//This is a header file for the Lexer (API)
//Makes functions from Lexer.c availible to Parser.c

#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

typedef struct {
    const char *source;
    int current_pos;
    int line;
    int column;
} Lexer;

//Public Lexer functions
Lexer* init_lexer(const char *source);
Token* next_token(Lexer *lexer);
void free_token(Token *token);

#endif