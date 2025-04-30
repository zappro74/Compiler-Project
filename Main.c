#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"

int main()
{
    Lexer* lexer = init_lexer("int x = 2 + 20");

    while (1)
    {
        Token* token = next_token(lexer);
        printf("Token: %s\n", token->value);
        if (token -> type == TOKEN_EOF)
        {
            free_token(token);
            break;
        }

        free_token(token);
    }
}