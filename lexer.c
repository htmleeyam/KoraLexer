
#include "lexer.h"
#include <string.h>
#include <ctype.h>

// Convert token types into readable names
const char* token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_TYPE: return "TYPE";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_BOOLEAN: return "BOOLEAN";
        case TOKEN_PITCH: return "PITCH";
        case TOKEN_INTERVAL_UNIT: return "INTERVAL_UNIT";
        case TOKEN_STRING: return "STRING";

        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_POWER: return "POWER";

        case TOKEN_INCREMENT: return "INCREMENT";
        case TOKEN_DECREMENT: return "DECREMENT";

        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_MULTIPLY_ASSIGN: return "MULTIPLY_ASSIGN";
        case TOKEN_DIVIDE_ASSIGN: return "DIVIDE_ASSIGN";
        case TOKEN_MODULO_ASSIGN: return "MODULO_ASSIGN";

        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS_THAN: return "LESS";
        case TOKEN_GREATER_THAN: return "GREATER";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";

        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";

        case TOKEN_PIPELINE: return "PIPELINE";
        case TOKEN_RETURN_ARROW: return "RETURN_ARROW";
        case TOKEN_LAMBDA_ARROW: return "LAMBDA_ARROW";

        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE: return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "RIGHT_BRACE";
        case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";

        case TOKEN_COMMA: return "COMMA";
        case TOKEN_COLON: return "COLON";
        case TOKEN_DOT: return "DOT";
        case TOKEN_SEMICOLON: return "SEMICOLON";

        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";

        default: return "UNKNOWN";
    }
}

// Skip spaces and tabs automatically
void skip_whitespace(Lexer *lexer) {
    while (lexer->source[lexer->position] == ' ' ||
           lexer->source[lexer->position] == '\t') {
        lexer->position++;
        lexer->column++;
    }
}

// Check whether an identifier can start with this character
int is_identifier_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

// Check whether an identifier can contain this character
int is_identifier_part(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

// Check whether a word is a reserved Kora keyword
int is_keyword(const char *word) {
    const char *keywords[] = {
        "class", "struct", "track", "verse",
        "function", "pure", "new", "let",
        "const", "auto", "return", "if",
        "else", "for", "while", "break",
        "try", "catch", "throw", "tempo",
        "volume", "beat", "add", "synch"
    };

    int count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < count; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

// Read an identifier or keyword
Token read_identifier(Lexer *lexer) {
    Token token = {0};

    int start = lexer->position;
    int start_column = lexer->column;

    while (is_identifier_part(lexer->source[lexer->position])) {
        lexer->position++;
        lexer->column++;
    }

    int length = lexer->position - start;

    if (length >= 256) {
        length = 255;
    }

    strncpy(token.lexeme, lexer->source + start, length);
    token.lexeme[length] = '\0';

    token.line = lexer->line;
    token.column = start_column;

    if (is_keyword(token.lexeme)) {
        token.type = TOKEN_KEYWORD;
    } else {
        token.type = TOKEN_IDENTIFIER;
    }

    return token;
}

// Create a token from the current source position
Token make_token(Lexer *lexer, TokenType type, int length) {
    Token token = {0};

    int start = lexer->position;

    token.type = type;
    token.line = lexer->line;
    token.column = lexer->column;

    if (length >= 256) {
        length = 255;
    }

    for (int i = 0; i < length; i++) {
        token.lexeme[i] = lexer->source[start + i];
    }

    token.lexeme[length] = '\0';

    lexer->position += length;
    lexer->column += length;

    return token;
}


Token read_operator_or_delimiter(Lexer *lexer)
{
    char current = lexer->source[lexer->position];
    char next = lexer->source[lexer->position + 1];

    // Check two-character operators first (longest match).

    if (current == '+' && next == '=')
        return make_token(lexer, TOKEN_PLUS_ASSIGN, 2);

    if (current == '-' && next == '=')
        return make_token(lexer, TOKEN_MINUS_ASSIGN, 2);

    if (current == '*' && next == '=')
        return make_token(lexer, TOKEN_MULTIPLY_ASSIGN, 2);

    if (current == '/' && next == '=')
        return make_token(lexer, TOKEN_DIVIDE_ASSIGN, 2);

    if (current == '%' && next == '=')
        return make_token(lexer, TOKEN_MODULO_ASSIGN, 2);

    if (current == '*' && next == '*')
        return make_token(lexer, TOKEN_POWER, 2);

    if (current == '+' && next == '+')
        return make_token(lexer, TOKEN_INCREMENT, 2);

    if (current == '-' && next == '-')
        return make_token(lexer, TOKEN_DECREMENT, 2);

    if (current == '=' && next == '=')
        return make_token(lexer, TOKEN_EQUAL, 2);

    if (current == '!' && next == '=')
        return make_token(lexer, TOKEN_NOT_EQUAL, 2);

    if (current == '<' && next == '=')
        return make_token(lexer, TOKEN_LESS_EQUAL, 2);

    if (current == '>' && next == '=')
        return make_token(lexer, TOKEN_GREATER_EQUAL, 2);

    if (current == '&' && next == '&')
        return make_token(lexer, TOKEN_AND, 2);

    if (current == '|' && next == '|')
        return make_token(lexer, TOKEN_OR, 2);

    if (current == '|' && next == '>')
        return make_token(lexer, TOKEN_PIPELINE, 2);

    if (current == '-' && next == '>')
        return make_token(lexer, TOKEN_RETURN_ARROW, 2);

    if (current == '=' && next == '>')
        return make_token(lexer, TOKEN_LAMBDA_ARROW, 2);


    // Check single-character operators and delimiters.

    switch (current)
    {
        case '+': return make_token(lexer, TOKEN_PLUS, 1);
        case '-': return make_token(lexer, TOKEN_MINUS, 1);
        case '*': return make_token(lexer, TOKEN_MULTIPLY, 1);
        case '/': return make_token(lexer, TOKEN_DIVIDE, 1);
        case '%': return make_token(lexer, TOKEN_MODULO, 1);

        case '=': return make_token(lexer, TOKEN_ASSIGN, 1);
        case '<': return make_token(lexer, TOKEN_LESS_THAN, 1);
        case '>': return make_token(lexer, TOKEN_GREATER_THAN, 1);
        case '!': return make_token(lexer, TOKEN_NOT, 1);

        case '(': return make_token(lexer, TOKEN_LEFT_PAREN, 1);
        case ')': return make_token(lexer, TOKEN_RIGHT_PAREN, 1);

        case '{': return make_token(lexer, TOKEN_LEFT_BRACE, 1);
        case '}': return make_token(lexer, TOKEN_RIGHT_BRACE, 1);

        case '[': return make_token(lexer, TOKEN_LEFT_BRACKET, 1);
        case ']': return make_token(lexer, TOKEN_RIGHT_BRACKET, 1);

        case ',': return make_token(lexer, TOKEN_COMMA, 1);
        case ':': return make_token(lexer, TOKEN_COLON, 1);
        case '.': return make_token(lexer, TOKEN_DOT, 1);
        case ';': return make_token(lexer, TOKEN_SEMICOLON, 1);

        default:
            return make_token(lexer, TOKEN_ERROR, 1);
    }
}

// Reads numbers whether it is an integer or a float
Token read_number(Lexer *lexer)
{
    int length = 0;
    TokenType type = TOKEN_INTEGER;

    // Read whole-number digits.
    while (isdigit(lexer->source[lexer->position + length]))
    {
        length++;
    }

    // Check for a decimal point followed by a digit.
    if (lexer->source[lexer->position + length] == '.' &&
        isdigit(lexer->source[lexer->position + length + 1]))
    {
        type = TOKEN_FLOAT;
        length++; // Include the decimal point.

        // Read digits after the decimal point.
        while (isdigit(lexer->source[lexer->position + length]))
        {
            length++;
        }
    }

    return make_token(lexer, type, length);
}

// Main function for retrieving the next token
Token get_next_token(Lexer *lexer)
{
    skip_whitespace(lexer);

    char current = lexer->source[lexer->position];

    // End of source.
    if (current == '\0')
    {
        return make_token(lexer, TOKEN_EOF, 0);
    }

    // Identifiers and keywords.
    if (is_identifier_start(current))
    {
        return read_identifier(lexer);
    }

    // Integer and float literals.
    if (isdigit(current))
    {
        return read_number(lexer);
    }

    // Operators and delimiters.
    return read_operator_or_delimiter(lexer);
}