#include <stdio.h> // I/O
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_KEYWORD,     
    TOKEN_OPERATOR,    
    TOKEN_PUNCTUATION, 
    TOKEN_EOF,         
    TOKEN_ERROR  
} TokenType;

typedef struct 
{
    const char *keyword;
    TokenType type;
} Keyword; //making a keyword struct for a keyword table

//keyword table, used in the read_identifier() method to distinguish keyword from identifier.
static const Keyword keywords[] //keywords cannot be used as an identifier. identifiers are variables that are named whatever besides these keywords.
{
    {"if", TOKEN_KEYWORD_IF},
    {"else", TOKEN_KEYWORD_ELSE},
    {"for", TOKEN_KEYWORD_FOR},
    {"while", TOKEN_KEYWORD_WHILE},
    {"do", TOKEN_KEWORD_DO},
    {"return", TOKEN_KEYWORD_RETURN},
    {"new", TOKEN_KEYWORD_NEW},
    {"switch", TOKEN_KEYWORD_SWITCH},
    {"continue", TOKEN_KEYWORD_CONTINUE},
    {"abstract", TOKEN_KEYWORD_ABSTRACT},
    {"default", TOKEN_KEYWORD_DEFAULT},
    {"assert", TOKEN_KEYWORD_ASSERT},
    {"goto", TOKEN_KEYWORD_GOTO},
    {"package", TOKEN_KEYWORD_PACKAGE},
    {"synchronized", TOKEN_KEYWORD_SYNCHRONIZED},
    {"boolean", TOKEN_KEYWORD_BOOLEAN},
    {"private", TOKEN_KEYWORD_PRIVATE},
    {"this", TOKEN_KEYWORD_THIS},
}

typedef struct
{
    TokenType type; //Find what token it is
    char *value;    //Token value
    int line;       //What line is it in the file
    int column;
} Token; 

typedef struct
{
    const char *source; //Whole code
    int current_pos;    //... pretty obvious
    int line;           //where ma errors at
    int column;
} Lexer; //detects where errors are in code

Lexer* init_lexer(const char *source) //had to search this one up
{
    Lexer *lexer = (lexer*)malloc(siseof(lexer));
    if (!Lexer)
    {
        fprintf(stderr, "memory allocation failed\n");
        exit(1);
    }

    lexer -> source = source;
    lexer -> current_pos = 0;
    lexer -> line = 1;
    lexer -> column = 1;

    return lexer;
}

void advance(Lexer *lexer)
{
    if (lexer -> source[lexer -> current_pos] == \n) //go to the struct lexer and grabs source. Then in the source, find the current position of the new line.
    {
        lexer -> line++; //go to next line
        lexer -> column = 1; //reset column at new line
    }
    else
    {
        lexer -> column++; //continue to next column
    }
    lexer -> current_pos++; //always advances, would get stuck without it 
}

char peek(Lexer *lexer) //returns current character without advancing
{
    return lexer -> source[lexer -> current_pos]; //charAt(i)
}

char peek_next(Lexer *lexer)
{
    return lexer -> source[lexer -> current_pos + 1]; //charAt(i + 1)
}

int is_whitespace(char c) //check if current position is whitespace
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

void skip_whitespace(Lexer *lexer)
{
    while (is_whitespace(peek(lexer)))
    {
        advance(lexer);
    }
}

Token* read_identifier(Lexer *lexer)
{
    
}