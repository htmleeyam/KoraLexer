
#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_TYPE,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_BOOLEAN,
    TOKEN_PITCH,
    TOKEN_INTERVAL_UNIT,
    TOKEN_STRING,

    // Arithmetic operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_POWER,

    // Increment and decrement
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,

    // Assignment operators
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,

    // Comparison operators
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,

    // Logical operators
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    // Special operators
    TOKEN_PIPELINE,
    TOKEN_RETURN_ARROW,
    TOKEN_LAMBDA_ARROW,

    // Brackets and delimiters
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_SEMICOLON,

    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR

} TokenType;

typedef struct {
    TokenType type;
    char lexeme[256];
    int line;
    int column;
} Token;

typedef struct {
    const char *source;
    int position;
    int line;
    int column;
} Lexer;

// Function declarations
const char* token_type_name(TokenType type);

void skip_whitespace(Lexer *lexer);

int is_identifier_start(char c);
int is_identifier_part(char c);
int is_pitch_literal(Lexer *lexer);
TokenType classify_identifier(const char *word);

Token read_identifier(Lexer *lexer);
Token make_token(Lexer *lexer, TokenType type, int length);
Token get_next_token(Lexer *lexer);
Token read_operator_or_delimiter(Lexer *lexer);
Token read_number(Lexer *lexer);

#endif