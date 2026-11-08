#include <stdio.h>
#include "lexer.h"

int main()
{
    Lexer lexer = {
    .source =
        "\"Hello {name}!\"\n"
        "\"Score: {100 + 20}\"\n"
        "\"Result: {calculate({value})}\"\n"
        "\"Literal \\{ brace\"\n"
        "\"Broken: {name\n"
        "let recovered = 123\n"
        "\"Back to normal {C4}\"\n",


        .position = 0,
        .line = 1,
        .column = 1,

        .inside_block_comment = 0,
        .block_comment_start_line = 0,
        .block_comment_start_column = 0,

        .inside_string = 0,
        .inside_interpolation = 0,
        .interpolation_brace_depth = 0
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