#include <stdio.h>
#include "lexer.h"

int main()
{
    Lexer lexer = {
        .source =
            "\"Hello Kora\"\n"
            "\"Hello\\nWorld\"\n"
            "\"She said \\\"Hello\\\"\"\n"
            "\"Path: C:\\\\Kora\"\n"
            "\"Tab\\tSeparated\"\n"
            "\"Carriage\\rReturn\"\n"
            "\"Literal \\{ brace\"\n"
            "\"Invalid \\q\"\n",

        .position = 0,
        .line = 1,
        .column = 1,

        .inside_block_comment = 0,
        .block_comment_start_line = 0,
        .block_comment_start_column = 0
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