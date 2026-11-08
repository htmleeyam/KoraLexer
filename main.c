#include <stdio.h>
#include "lexer.h"

int main()
{
    Lexer lexer = {
        "var123abc\n"
        "_note456\n"
        "123abc\n"
        "456xyz\n"
        "12.34.56\n"
        "78.90",
        0, 1, 1, 0, 0, 0
    };

    Token token;

    printf("\n");
    printf("===============================================\n");
    printf("             KORA LEXICAL ANALYZER             \n");
    printf("===============================================\n");

    printf("%-20s %-20s\n", "LEXEME", "TOKEN");

    printf("-----------------------------------------------\n");

    do
    {
        token = get_next_token(&lexer);

        if (token.type != TOKEN_EOF)
        {
            printf("%-20s %-20s\n",
                token.lexeme,
                token_type_name(token.type));
        }

    } while (token.type != TOKEN_EOF);

    printf("-----------------------------------------------\n");

    return 0;
}