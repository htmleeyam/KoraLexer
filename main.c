#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// Terminal colors (ANSI escape codes).
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[92m"
#define PURPLE  "\x1b[35m"
#define GRAY    "\x1b[90m"
#define WHITE   "\x1b[37m"

// Check whether the input file has a .kora extension.
int has_kora_extension(const char *filename) {
    const char *extension = strrchr(filename, '.');

    if (extension == NULL) {
        return 0;
    }

    return strcmp(extension, ".kora") == 0;
}

// Read the entire source file into memory.
char *read_source_file(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        printf(RED "Error: Could not open file '%s'.\n" RESET, filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0) {
        printf(RED "Error: Could not determine file size.\n" RESET);
        fclose(file);
        return NULL;
    }

    char *source = malloc((size_t)file_size + 1);

    if (source == NULL) {
        printf(RED "Error: Memory allocation failed.\n" RESET);
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(source, 1, (size_t)file_size, file);

    if (bytes_read != (size_t)file_size) {
        printf(RED "Error: Could not completely read source file.\n" RESET);
        free(source);
        fclose(file);
        return NULL;
    }

    source[bytes_read] = '\0';

    fclose(file);

    return source;
}

// Make special characters visible in the terminal.
void print_escaped_lexeme(const char *lexeme, char *output) {
    int j = 0;

    for (int i = 0; lexeme[i] != '\0' && j < 1000; i++) {
        switch (lexeme[i]) {
            case '\n':
                output[j++] = '\\';
                output[j++] = 'n';
                break;

            case '\r':
                output[j++] = '\\';
                output[j++] = 'r';
                break;

            case '\t':
                output[j++] = '\\';
                output[j++] = 't';
                break;

            case '\\':
                output[j++] = '\\';
                output[j++] = '\\';
                break;

            default:
                output[j++] = lexeme[i];
                break;
        }
    }

    output[j] = '\0';
}

// Print one token with lexeme first and token type second.
void print_token(Token token) {
    char escaped[1024];

    print_escaped_lexeme(token.lexeme, escaped);

    if (token.type == TOKEN_ERROR) {
        printf(RED "%-24s %-24s" RESET "\n",
            escaped,
            token_type_name(token.type));
    } else {
        printf(WHITE "%-24s %-24s" RESET "\n",
            escaped,
            token_type_name(token.type));
    }
}

// Display additional information for lexical errors.
void print_error_details(Token token) {
    char escaped[1024];

    print_escaped_lexeme(token.lexeme, escaped);

    printf(RED "\n  LEXICAL ERROR\n" RESET);

    printf(WHITE "  Description : " RED "%s\n" RESET,
        lexical_error_message(token.error_code));

    printf(WHITE "  Lexeme      : " RED "'%s'\n" RESET,
        escaped);

    printf(WHITE "  Location    : Line %d, Column %d\n" RESET,
        token.line,
        token.column);

    printf(WHITE "  Suggestion  : %s\n\n" RESET,
        lexical_error_suggestion(token.error_code));
}

int main(int argc, char *argv[]) {
    printf("\n");

    printf(WHITE
        "============================================================\n"
        "                  KORA LEXICAL ANALYZER                     \n"
        "============================================================\n"
        RESET);

    // Require a source file argument.
    if (argc < 2) {
        printf(RED "Usage: kora.exe <source_file.kora>\n" RESET);
        printf(GRAY "Example: kora.exe examples/test_features.kora\n" RESET);
        return 1;
    }

    if (argc > 2) {
        printf(RED "Error: Please provide only one source file.\n" RESET);
        return 1;
    }

    const char *filename = argv[1];

    // Validate file extension.
    if (!has_kora_extension(filename)) {
        printf(RED "Error: Invalid file extension.\n" RESET);
        printf(WHITE "Please provide a .kora source file.\n" RESET);
        return 1;
    }

    // Read source file.
    char *source = read_source_file(filename);

    if (source == NULL) {
        return 1;
    }

    // Display the file being processed.
    printf(WHITE "Processing File: " RESET PURPLE "%s\n\n" RESET, filename);

    // Display token table header.
    printf(GRAY
        "------------------------------------------------------------\n"
        RESET);

    printf(WHITE "%-24s %-24s\n" RESET,
        "LEXEME",
        "TOKEN TYPE");

    printf(GRAY
        "------------------------------------------------------------\n"
        RESET);

    // Initialize lexer.
    Lexer lexer = {
        .source = source,
        .position = 0,
        .line = 1,
        .column = 1,
        .inside_block_comment = 0,
        .block_comment_start_line = 0,
        .block_comment_start_column = 0,
        .context_depth = 0
    };

    // Retrieve and display tokens.
    Token token;
    int error_count = 0;
    int token_count = 0;

    do {
        token = get_next_token(&lexer);

        if (token.type != TOKEN_EOF) {
            print_token(token);
            token_count++;

            if (token.type == TOKEN_ERROR) {
                error_count++;
                print_error_details(token);
            }
        }

    } while (token.type != TOKEN_EOF);

    // Display token table footer.
    printf(GRAY
        "------------------------------------------------------------\n"
        RESET);

    // Display lexical analysis summary.
    printf(WHITE "\nLEXICAL ANALYSIS SUMMARY\n" RESET);

    printf(WHITE "Total tokens: " GREEN "%d\n" RESET, token_count);

    if (error_count > 0) {
        printf(WHITE "Lexical errors: " RED "%d\n" RESET, error_count);
        printf(RED "Lexical analysis completed with errors.\n" RESET);
    } else {
        printf(WHITE "Lexical errors: " GREEN "%d\n" RESET, error_count);
        printf(GREEN "Lexical analysis completed successfully.\n" RESET);
    }

    printf(GRAY
        "============================================================\n"
        RESET);

    // Release allocated memory.
    free(source);

    return 0;
}