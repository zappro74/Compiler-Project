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

int is_keyword(const char *lexeme) 
{
    static const char *keyword_list[] = 
    {
        "abstract", "continue", "for", "new", "switch",
        "assert", "default", "goto", "package", "synchronized",
        "boolean", "do", "if", "private", "this",
        "break", "double", "implements", "protected", "throw",
        "byte", "else", "import", "public", "throws",
        "case", "enum", "instanceof", "return", "transient",
        "catch", "extends", "int", "short", "try",
        "char", "final", "interface", "static", "void",
        "class", "finally", "long", "strictfp", "volatile",
        "const", "float", "native", "super", "while"
    };
    
    int num_keywords = sizeof(keyword_list) / sizeof(keyword_list[0]);

    for (int i = 0; i < num_keywords; i++) 
    {
        if (strcmp(lexeme, keyword_list[i]) == 0) 
        {
            return 1; // Is a keyword
        }
    }
    return 0; // Not a keyword
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

int skip_whitespace(Lexer *lexer)
{
    int skipped = 0;
    while (is_whitespace(peek(lexer)))
    {
        advance(lexer);
        skipped = 1;
    }
    return skipped;
}

int skip_comment(Lexer *lexer)
{
    int skipped = 0;
    if (peek(lexer) == '/' && (peek_next(lexer) == '/' || peek_next(lexer) == '*'))
    {
        int start_line = lexer -> line;

        if (peek_next(lexer) == '/')
        {
            advance(lexer); //skip //
            advance(lexer);
            while (peek(lexer) != '\n' && peek(lexer) != '\0')
            {
                advance(lexer);
                skipped = 1;
            }
        }
        else
        {
            advance(lexer); //skip /*
            advance(lexer);
            while (!(peek(lexer) == '*' && peek_next(lexer) == '/'))
            {
                if (peek(lexer) == '\0') //this comment could technically go to the EOF
                {
                    return skipped;
                }
                advance(lexer);
                skipped = 1;
            }
            advance(lexer); //skip */
            advance(lexer);
        }
    }
    return skipped;
}

int is_identifier_start(char c)
{
    return isalpha(c) || c == '_' || c == '$';
}

int is_identifier(char c)
{
    return isalpha(c) || isdigit(c) || c == '_' || c == '$';
}

int is_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '<' || c == '>' || c == '!' || c == '&' ||
           c == '|' || c == '^' || c == '~' || c == '?' || c == ':'; 
}

int is_punctuation(char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}' ||
           c == '[' || c == ']' || c == ';' || c == ',' ||
           c == '.' || c == '@' || c == ':';
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

    TokenType type = is_keyword(lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;

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

Token* read_punctuation(Lexer *lexer)
{
    if (!is_punctuation(peek(lexer)))
    {
        return NULL;
    }

    int start_pos = lexer->current_pos;
	int start_line = lexer->line;
	int start_column = lexer->column;
    char punc = peek(lexer); //The punctuation character

    advance(lexer);

    //Extract lexme
    char *lexeme = malloc(2);

    if (!lexeme)
    {
        fprintf(stderr, "Memory error: Could not allocate lexeme\n");
        return NULL;
    }

    lexeme[0] = punc;
    lexeme[1] = '\0';

    TokenType type = TOKEN_PUNCTUATION;

    return create_token(type, lexeme, start_line, start_column);
}

Token* read_operator(Lexer *lexer)
{
	if (!is_operator(peek(lexer)))
	{
		return NULL;
	}

	int start_pos = lexer->current_pos;
	int start_line = lexer->line;
	int start_column = lexer->column;

	char current = peek(lexer); 
	char next = peek_next(lexer);
	char next2 = lexer->source[lexer->current_pos + 2];  // Lookahead for 3-char operators

	int op_length = 1;  // Default to single-char operator

    //the following if statement chain was written by AI because... I aint writing all that.
	if (current == '=' && next == '=') 
    {
		op_length = 2;  // ==
	} 
    else if (current == '!' && next == '=') 
    {
		op_length = 2;  // !=
	} 
    else if (current == '<') 
    {
		if (next == '=') 
        {
			op_length = 2;  // <=
		} 
        else if (next == '<') 
        {
			if (next2 == '=') 
            {
				op_length = 3;  // <<=
			} 
            else 
            {
				op_length = 2;  // <<
			}
		}
	} 
    else if (current == '>') 
    {
		if (next == '=') 
        {
			op_length = 2;  // >=
		} 
        else if (next == '>') 
        {
			if (next2 == '=') 
            {
				op_length = 3;  // >>=
			} 
            else if (lexer->source[lexer->current_pos + 3] == '=') 
            {
				op_length = 4;  // >>>=
			} 
            else if (next2 == '>') 
            {
				op_length = 3;  // >>>
			} 
            else 
            {
				op_length = 2;  // >>
			}
		}
	} 
    else if (current == '+' && next == '+') 
    {
		op_length = 2;  // ++
	} 
    else if (current == '+' && next == '=') 
    {
		op_length = 2;  // +=
	} 
    else if (current == '-' && next == '-') 
    {
		op_length = 2;  // --
	} 
    else if (current == '-' && next == '=') 
    {
		op_length = 2;  // -=
	} 
    else if (current == '*' && next == '=') 
    {
		op_length = 2;  // *=
	} 
    else if (current == '/' && next == '=') 
    {
		op_length = 2;  // /=
	} 
    else if (current == '%' && next == '=') 
    {
		op_length = 2;  // %=
	} 
    else if (current == '&') 
    {
		if (next == '&') 
        {
			op_length = 2;  // &&
		} 
        else if (next == '=') 
        {
			op_length = 2;  // &=
		}
	} 
    else if (current == '|') 
    {
		if (next == '|') 
        {
			op_length = 2;  // ||
		} 
        else if (next == '=') 
        {
			op_length = 2;  // |=
		}
	} 
    else if (current == '^' && next == '=') 
    {
		op_length = 2;  // ^=
	} 
    else if (current == ':') 
    {
		op_length = 1;  // :
	} 
    else if (current == '?') 
    {
		op_length = 1;  // ?
	} 
    else if (current == '~') 
    {
		op_length = 1;  // ~
	}

	// Advance lexer
	for (int i = 0; i < op_length; i++) {
		advance(lexer);
	}

	// Extract lexeme
	int length = lexer->current_pos - start_pos;
	char *lexeme = malloc(length + 1);

	if (!lexeme)
	{
		fprintf(stderr, "Memory error: Could not allocate lexeme\n");
		return NULL;
	}

	strncpy(lexeme, lexer->source + start_pos, length);
	lexeme[length] = '\0';

	TokenType type = TOKEN_OPERATOR;

	return create_token(type, lexeme, start_line, start_column);
}

Token* next_token(Lexer *lexer)
{
    while (skip_whitespace(lexer) || skip_comment(lexer))
    {
        //this keeps going until there is no whitespace or comments to skip
        //it's a little wierd, but I wanted to use my skip_whitespace() function :)
    }

    if (peek(lexer) == '\0')
    {
        return create_token(TOKEN_EOF, "End Of File", lexer -> line, lexer -> column);
    }

    int start_pos = lexer -> current_pos;
    int start_line = lexer -> line;
    int start_column = lexer -> column;

    Token* token = read_identifier(lexer);
    if (token != NULL)
    {
        return token;
    }

    token = read_number(lexer);
    if (token != NULL)
    {
        return token;
    }

    token = read_string(lexer);
    if (token != NULL)
    {
        return token;
    }

    token = read_operator(lexer);
    if (token != NULL)
    {
        return token;
    }

    token = read_punctuation(lexer);
    if (token != NULL)
    {
        return token;
    }
    
    char error_char[2] = { peek(lexer), '\0' }; //This section gives the exact character that caused the error
    advance(lexer); // Move past said character
    return create_token(TOKEN_ERROR, error_char, start_line, start_column);
}