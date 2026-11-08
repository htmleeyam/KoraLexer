#include <stdio.h>
#include "lexer.h"

int main()
{
    Lexer lexer = {
        .source = "let tempo = 120\n"
                "constant volume = 80\n"
                "auto let melody = C4\n"
                "let duration = 2 bt\n"
                "let interval = 3 semitones\n"
                "let enabled = true\n"
                "print(melody)\n"
                "sync\n"
                "synchronize\n",

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