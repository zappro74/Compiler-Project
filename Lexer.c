#include <stdio.h> // I/O
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //for isalpha()/isdigit() method

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
    {"abstract", TOKEN_KEYWORD_ABSTRACT},
    {"continue", TOKEN_KEYWORD_CONTINUE},
    {"for", TOKEN_KEYWORD_FOR},
    {"new", TOKEN_KEYWORD_NEW},
    {"switch", TOKEN_KEYWORD_SWITCH},
    {"assert", TOKEN_KEYWORD_ASSERT},
    {"default", TOKEN_KEYWORD_DEFAULT},
    {"goto", TOKEN_KEYWORD_GOTO},
    {"package", TOKEN_KEYWORD_PACKAGE},
    {"synchronized", TOKEN_KEYWORD_SYNCHRONIZED},
    {"boolean", TOKEN_KEYWORD_BOOLEAN},
    {"do", TOKEN_KEYWORD_DO},
    {"if", TOKEN_KEYWORD_IF},
    {"private", TOKEN_KEYWORD_PRIVATE},
    {"this", TOKEN_KEYWORD_THIS},
    {"break", TOKEN_KEYWORD_BREAK},
    {"double", TOKEN_KEYWORD_DOUBLE},
    {"implements", TOKEN_KEYWORD_IMPLEMENTS},
    {"protected", TOKEN_KEYWORD_PROTECTED},
    {"throw", TOKEN_KEYWORD_THROW},
    {"byte", TOKEN_KEYWORD_BYTE},
    {"else", TOKEN_KEYWORD_ELSE},
    {"import", TOKEN_KEYWORD_IMPORT},
    {"public", TOKEN_KEYWORD_PUBLIC},
    {"throws", TOKEN_KEYWORD_THROWS},
    {"case", TOKEN_KEYWORD_CASE},
    {"enum", TOKEN_KEYWORD_ENUM},
    {"instanceof", TOKEN_KEYWORD_INSTANCEOF},
    {"return", TOKEN_KEYWORD_RETURN},
    {"transient", TOKEN_KEYWORD_TRANSIENT},
    {"catch", TOKEN_KEYWORD_CATCH},
    {"extends", TOKEN_KEYWORD_EXTENDS},
    {"int", TOKEN_KEYWORD_INT},
    {"short", TOKEN_KEYWORD_SHORT},
    {"try", TOKEN_KEYWORD_TRY},
    {"char", TOKEN_KEYWORD_CHAR},
    {"final", TOKEN_KEYWORD_FINAL},
    {"interface", TOKEN_KEYWORD_INTERFACE},
    {"static", TOKEN_KEYWORD_STATIC},
    {"void", TOKEN_KEYWORD_VOID},
    {"class", TOKEN_KEYWORD_CLASS},
    {"finally", TOKEN_KEYWORD_FINALLY},
    {"long", TOKEN_KEYWORD_LONG},
    {"strictfp", TOKEN_KEYWORD_STRICTFP},
    {"volatile", TOKEN_KEYWORD_VOLATILE},
    {"const", TOKEN_KEYWORD_CONST},
    {"float", TOKEN_KEYWORD_FLOAT},
    {"native", TOKEN_KEYWORD_NATIVE},
    {"super", TOKEN_KEYWORD_SUPER},
    {"while", TOKEN_KEYWORD_WHILE}
}

/*
static const Number numbers[]
{
    {"0", TOKEN_NUMBER_ZERO}
    {"1", TOKEN_NUMBER_ONE}
    {"2", TOKEN_NUMBER_TWO}
    {"3", TOKEN_NUMBER_THREE}
    {"4", TOKEN_NUMBER_FOUR}
    {"5", TOKEN_NUMBER_FIVE}
    {"6", TOKEN_NUMBER_SIX}
    {"7", TOKEN_NUMBER_SEVEN}
    {"8", TOKEN_NUMBER_EIGHT}
    {"9", TOKEN_NUMBER_NINE}
} */

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

Token* create_token(TokenType type, char* lexeme, int line, int column) //creates a token with the inputs of the TokenType, the raw string, and the location in the source
{
    Token* token = (Token*)malloc(sizeof(Token));

    if (!token)
    {
        fprintf(stderr, "Error: Failed to allocate memory for Token\n");
        return NULL;
    }

    char* lexeme_copy = (char*)malloc(strlen(lexeme) + 1); //add 1 to the string length because strings are null terminated in C. which means there is an extra byte at the end of a string.
    if (!lexeme_copy)
    {
        fprintf(stderr, "Error: Failed to allocate memory for lexeme\n");
        free(token); //free the memory location of the token struct, since there is no longer a token struct being created
        return NULL;
    }

    strcpy(lexeme_copy, lexeme); //copies content from lexeme to lexeme_copy

    token -> type = type;
    token -> value = lexeme_copy;
    token -> line = line;
    token -> column = column;

    return token;
}

Lexer* init_lexer(const char *source)
{
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer)); //allocate memory for the lexer
    if (!lexer)
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
    if (lexer -> source[lexer -> current_pos] == '\n') //go to the struct lexer and grabs source. Then in the source, find the current position of the new line.
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

int is_identifier_start(char c)
{
    return isalpha(c) || c == "_" || c == '$';
}

int is_identifier(char c)
{
    return isalpha(c) || isdigit(c) || c == "_" || c == '$';
}

Token* read_identifier(Lexer *lexer)
{
    if (!is_identifier_start(peek(lexer)))
    {
        return NULL;
    }
    
    int start_pos = lexer -> current_pos;
    int start_line = lexer -> line;
    int start_column = lexer -> column;

    while (is_identifier(peek(lexer)))
    {
        advance(lexer);
    }

    //start of lexme(raw text that forms a token) extraction
    int length = lexer -> current_pos - start_pos;
    char *lexeme = malloc(length + 1);
    
    if (!lexeme) //check if memory allocation failed
    {
        fprintf(stderr, "Memory error: Could not allocate lexeme\n");
        return NULL;
    }

    strncpy(lexeme, lexer -> source + start_pos, length); //copies identifier from the source into lexeme buffer
    lexeme[length] = '\0'; //null termination again

    TokenType type = TOKEN_IDENTIFIER; //assuming whats in the lexeme is an identifier

    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(lexeme, keywords[i].keyword) == 0) //compares strings
        {
            type = keywords[i].type;
            break;
        }
    }

    return create_token(type, lexeme, start_line, start_column);
}

Token* read_number(Lexer *lexer)
{
    if (!isdigit(peek(lexer)))
    {
        return NULL;
    }

    int start_pos = lexer -> current_pos;
    int start_line = lexer -> line;
    int start_column = lexer -> column;

    while (isdigit(peek(lexer)))
    {
        advance(lexer);
    }

    //Extract lexme
    int length = lexer -> current_pos - start_pos;
    char *lexeme = malloc(length + 1);

    if (!lexeme)
    {
        fprintf(stderr, "Memory error: Could not allocate lexeme\n");
        return NULL;
    }

    strncpy(lexeme, lexer -> source + start_pos, length); 
    lexeme[length] = '\0';

    TokenType type = TOKEN_NUMBER;

    return create_token(type, lexeme, start_line, start_column);
}

Token* read_string(Lexer *lexer) //only works for main case so far, no '\n' detection or anything like that
{
    if (peek(lexer) != '"')
    {
        return NULL;
    }
    advance(lexer);

    int start_pos = lexer -> current_pos;
    int start_line = lexer -> line;
    int start_column = lexer -> column;

    while (peek(lexer) != '"' && peek(lexer) != '\0') 
    {
        advance(lexer);
    }

    if (peek(lexer) != '"')
    {
        return NULL;
    }
    else
    {
        advance(lexer);
    }

    //Extract lexme
    int length = lexer -> current_pos - start_pos;
    char *lexeme = malloc(length + 1);

    if (!lexeme)
    {
        fprintf(stderr, "Memory error: Could not allocate lexeme\n");
        return NULL;
    }

    strncpy(lexeme, lexer -> source + start_pos, length); 
    lexeme[length] = '\0';

    TokenType type = TOKEN_STRING;

    return create_token(type, lexeme, start_line, start_column);
}


Token* next_token(Lexer *lexer)
{

}