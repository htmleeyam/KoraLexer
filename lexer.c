
#include "lexer.h"
#include <string.h>
#include <ctype.h>

// Convert token types into readable names.
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
        case TOKEN_DURATION_UNIT: return "DURATION_UNIT";

        case TOKEN_STRING_START: return "STRING_START";
        case TOKEN_STRING_TEXT: return "STRING_TEXT";
        case TOKEN_INTERPOLATION_START: return "INTERPOLATION_START";
        case TOKEN_INTERPOLATION_END: return "INTERPOLATION_END";
        case TOKEN_STRING_END: return "STRING_END";

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

// Skip spaces and tabs.
void skip_whitespace(Lexer *lexer) {
    while (lexer->source[lexer->position] == ' ' ||
           lexer->source[lexer->position] == '\t') {
        lexer->position++;
        lexer->column++;
    }
}

int is_identifier_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

int is_identifier_part(char c) {
    return isalnum((unsigned char)c) || c == '_';
}


// Classify reserved words.
TokenType classify_identifier(const char *word) {
    struct KeywordEntry {
        const char *word;
        TokenType type;
    };

    static const struct KeywordEntry reserved_words[] = {
        // Structure
        {"class", TOKEN_KEYWORD},
        {"struct", TOKEN_KEYWORD},
        {"track", TOKEN_KEYWORD},
        {"verse", TOKEN_KEYWORD},
        {"new", TOKEN_KEYWORD},

        // Declarations and functions
        {"const", TOKEN_KEYWORD},
        {"let", TOKEN_KEYWORD},
        {"auto", TOKEN_KEYWORD},
        {"function", TOKEN_KEYWORD},
        {"fn", TOKEN_KEYWORD},
        {"pure", TOKEN_KEYWORD},
        {"return", TOKEN_KEYWORD},

        // Music settings
        {"tempo", TOKEN_KEYWORD},
        {"volume", TOKEN_KEYWORD},
        {"beat", TOKEN_KEYWORD},
        {"add", TOKEN_KEYWORD},
        {"synch", TOKEN_KEYWORD},

        // Control flow
        {"if", TOKEN_KEYWORD},
        {"else", TOKEN_KEYWORD},
        {"for", TOKEN_KEYWORD},
        {"while", TOKEN_KEYWORD},
        {"break", TOKEN_KEYWORD},

        // Error handling
        {"try", TOKEN_KEYWORD},
        {"catch", TOKEN_KEYWORD},
        {"throw", TOKEN_KEYWORD},

        // Boolean and special values
        {"true", TOKEN_BOOLEAN},
        {"false", TOKEN_BOOLEAN},
        {"current", TOKEN_KEYWORD},

        // Data types
        {"int", TOKEN_TYPE},
        {"float", TOKEN_TYPE},
        {"boolean", TOKEN_TYPE},
        {"String", TOKEN_TYPE},
        {"pitch", TOKEN_TYPE},
        {"interval", TOKEN_TYPE},
        {"event", TOKEN_TYPE},
        {"pattern", TOKEN_TYPE},

        // Interval units
        {"st", TOKEN_INTERVAL_UNIT},
        {"oct", TOKEN_INTERVAL_UNIT}
    };

    int count = sizeof(reserved_words) / sizeof(reserved_words[0]);

    for (int i = 0; i < count; i++) {
        if (strcmp(word, reserved_words[i].word) == 0) {
            return reserved_words[i].type;
        }
    }

    return TOKEN_IDENTIFIER;
}


// Read an identifier or keyword.
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
    token.type = classify_identifier(token.lexeme);

    return token;
}

// Create a token from the current source position.
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

// Read operators and delimiters.
Token read_operator_or_delimiter(Lexer *lexer) {
    char current = lexer->source[lexer->position];
    char next = lexer->source[lexer->position + 1];

    // Two-character operators.
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

    // Single-character operators and delimiters.
    switch (current) {
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

// Read integer and float literals.
Token read_number(Lexer *lexer) {
    int length = 0;
    TokenType type = TOKEN_INTEGER;

    while (isdigit((unsigned char)lexer->source[lexer->position + length])) {
        length++;
    }

    if (lexer->source[lexer->position + length] == '.') {
        if (!isdigit((unsigned char)lexer->source[lexer->position + length + 1])) {
            length++;
            return make_token(lexer, TOKEN_ERROR, length);
        }

        type = TOKEN_FLOAT;
        length++;

        while (isdigit((unsigned char)lexer->source[lexer->position + length])) {
            length++;
        }

        if (lexer->source[lexer->position + length] == '.' &&
            isdigit((unsigned char)lexer->source[lexer->position + length + 1])) {
            while (isdigit((unsigned char)lexer->source[lexer->position + length]) ||
                   lexer->source[lexer->position + length] == '.') {
                length++;
            }

            return make_token(lexer, TOKEN_ERROR, length);
        }
    }

    if (is_identifier_part(lexer->source[lexer->position + length])) {
        while (is_identifier_part(lexer->source[lexer->position + length])) {
            length++;
        }

        return make_token(lexer, TOKEN_ERROR, length);
    }

    return make_token(lexer, type, length);
}

// Check whether the current position begins a pitch literal.
int is_pitch_literal(Lexer *lexer) {
    const char *source = lexer->source;
    int pos = lexer->position;

    char note = source[pos];

    if (note < 'A' || note > 'G') {
        return 0;
    }

    int offset = 1;

    if (source[pos + offset] == '#' || source[pos + offset] == 'b') {
        offset++;
    }

    if (source[pos + offset] == '-') {
        offset++;
    }

    if (!isdigit((unsigned char)source[pos + offset])) {
        return 0;
    }

    offset++;

    if (is_identifier_part(source[pos + offset])) {
        return 0;
    }

    return 1;
}


// Read a newline, treating CRLF as one logical newline.
// A standalone CR is an invalid character.
Token read_newline(Lexer *lexer) {
    Token token = {0};

    token.line = lexer->line;
    token.column = lexer->column;

    // Reject a CR that is not followed by LF.
    if (lexer->source[lexer->position] == '\r' &&
        lexer->source[lexer->position + 1] != '\n') {

        token.type = TOKEN_ERROR;
        token.lexeme[0] = '\r';
        token.lexeme[1] = '\0';

        // Consume the invalid character without advancing the line.
        lexer->position++;
        lexer->column++;

        return token;
    }

    // Valid LF or CRLF newline.
    token.type = TOKEN_NEWLINE;

    int length = 1;

    if (lexer->source[lexer->position] == '\r' &&
        lexer->source[lexer->position + 1] == '\n') {
        length = 2;
    }

    for (int i = 0; i < length; i++) {
        token.lexeme[i] = lexer->source[lexer->position + i];
    }

    token.lexeme[length] = '\0';

    lexer->position += length;
    lexer->line++;
    lexer->column = 1;

    return token;
}


// Read and skip Kora comments.
int read_comment(Lexer *lexer, Token *token) {
    const char *source = lexer->source;

    if (!lexer->inside_block_comment) {
        // Single-line comment.
        if (source[lexer->position] == '~' &&
            source[lexer->position + 1] == '~') {

            lexer->position += 2;
            lexer->column += 2;

            while (source[lexer->position] != '\0' &&
                   source[lexer->position] != '\n' &&
                   source[lexer->position] != '\r') {
                lexer->position++;
                lexer->column++;
            }

            return 0;
        }

        // Block comment.
        if (source[lexer->position] == '~' &&
            source[lexer->position + 1] == '*') {

            lexer->inside_block_comment = 1;
            lexer->block_comment_start_line = lexer->line;
            lexer->block_comment_start_column = lexer->column;

            lexer->position += 2;
            lexer->column += 2;
        } else {
            *token = make_token(lexer, TOKEN_ERROR, 1);
            return 1;
        }
    }

    while (lexer->inside_block_comment) {
        char current = source[lexer->position];

        if (current == '\0') {
            *token = (Token){0};

            token->type = TOKEN_ERROR;
            token->line = lexer->block_comment_start_line;
            token->column = lexer->block_comment_start_column;

            strcpy(token->lexeme, "~*");

            lexer->inside_block_comment = 0;
            return 1;
        }

        if (current == '\n' || current == '\r') {
            *token = read_newline(lexer);
            return 1;
        }

        if (current == '*' && source[lexer->position + 1] == '~') {
            lexer->position += 2;
            lexer->column += 2;
            lexer->inside_block_comment = 0;
            return 0;
        }

        lexer->position++;
        lexer->column++;
    }

    return 0;
}

// Push a new lexer context onto the stack.
static int push_context(
    Lexer *lexer,
    LexerContextType type,
    int brace_depth
)
{
    if (lexer->context_depth >= MAX_LEXER_CONTEXTS)
    {
        return 0;
    }

    LexerContext *context =
        &lexer->context_stack[lexer->context_depth];

    context->type = type;
    context->brace_depth = brace_depth;

    lexer->context_depth++;

    return 1;
}

// Remove the most recent lexer context.
static void pop_context(Lexer *lexer)
{
    if (lexer->context_depth > 0)
    {
        lexer->context_depth--;
    }
}

// Get the current lexer context.
static LexerContext *current_context(Lexer *lexer)
{
    if (lexer->context_depth == 0)
    {
        return NULL;
    }

    return &lexer->context_stack[lexer->context_depth - 1];
}

// Read text inside a string.

Token read_string_content(Lexer *lexer)
{
    int length = 0;

    while (lexer->source[lexer->position + length] != '\0')
    {
        char current = lexer->source[lexer->position + length];

        // Stop at string or interpolation boundaries.
        if (current == '"' || current == '{' ||
            current == '\n' || current == '\r')
        {
            break;
        }

        // Validate escape sequences.
        if (current == '\\')
        {
            char escaped = lexer->source[lexer->position + length + 1];

            if (escaped == '\\' || escaped == '"' ||
                escaped == '\'' ||
                escaped == 'n' || escaped == 't' ||
                escaped == 'r' || escaped == '{')
            {
                length += 2;
                continue;
            }

            // Return preceding text before reporting the error.
            if (length > 0)
            {
                return make_token(lexer, TOKEN_STRING_TEXT, length);
            }

            int error_length = (escaped == '\0') ? 1 : 2;
            return make_token(lexer, TOKEN_ERROR, error_length);
        }

        length++;
    }

    // Return accumulated string text.
    if (length > 0)
    {
        return make_token(lexer, TOKEN_STRING_TEXT, length);
    }

    char current = lexer->source[lexer->position];

    // Start of interpolation.
    if (current == '{')
    {
        Token token = make_token(
            lexer,
            TOKEN_INTERPOLATION_START,
            1
        );

        if (!push_context(
                lexer,
                LEXER_CONTEXT_INTERPOLATION,
                1
            ))
        {
            token.type = TOKEN_ERROR;
        }

        return token;
    }

    // End of the current string.
    if (current == '"')
    {
        Token token = make_token(
            lexer,
            TOKEN_STRING_END,
            1
        );

        pop_context(lexer);

        return token;
    }

    // Unescaped newline inside a string.
    if (current == '\n' || current == '\r')
    {
        Token token = read_newline(lexer);
        token.type = TOKEN_ERROR;

        pop_context(lexer);

        return token;
    }

    // Unterminated string at EOF.
    pop_context(lexer);

    return make_token(lexer, TOKEN_ERROR, 0);
}


// Main function for retrieving the next token.
Token get_next_token(Lexer *lexer)
{
    Token comment_token;

    // Check whether the current context is a string.
    LexerContext *context = current_context(lexer);

    if (context != NULL &&
        context->type == LEXER_CONTEXT_STRING)
    {
        return read_string_content(lexer);
    }

    // Skip whitespace and comments.
    while (1)
    {
        skip_whitespace(lexer);

        if (lexer->inside_block_comment ||
            lexer->source[lexer->position] == '~')
        {
            if (read_comment(lexer, &comment_token))
            {
                return comment_token;
            }

            continue;
        }

        break;
    }

    char current = lexer->source[lexer->position];

    // End of source.
    if (current == '\0')
    {
        if (lexer->context_depth > 0)
        {
            // An open string or interpolation was never closed.
            lexer->context_depth = 0;

            return make_token(lexer, TOKEN_ERROR, 0);
        }

        return make_token(lexer, TOKEN_EOF, 0);
    }

    // Newline handling.
    if (current == '\n' || current == '\r')
    {
        context = current_context(lexer);

        // A newline inside an interpolation is an error.
        if (context != NULL &&
            context->type == LEXER_CONTEXT_INTERPOLATION)
        {
            Token token = read_newline(lexer);
            token.type = TOKEN_ERROR;

            // Reset unfinished string/interpolation contexts.
            lexer->context_depth = 0;

            return token;
        }

        return read_newline(lexer);
    }

    // Start of a new string.
    if (current == '"')
    {
        Token token = make_token(
            lexer,
            TOKEN_STRING_START,
            1
        );

        if (!push_context(
                lexer,
                LEXER_CONTEXT_STRING,
                0
            ))
        {
            token.type = TOKEN_ERROR;
        }

        return token;
    }

    // Pitch literals.
    if (is_pitch_literal(lexer))
    {
        int start = lexer->position;
        int length = 1;

        if (lexer->source[start + length] == '#' ||
            lexer->source[start + length] == 'b')
        {
            length++;
        }

        if (lexer->source[start + length] == '-')
        {
            length++;
        }

        length++;

        return make_token(lexer, TOKEN_PITCH, length);
    }

    // Incomplete pitch literals.
    if (current >= 'A' && current <= 'G' &&
        (lexer->source[lexer->position + 1] == '#' ||
         lexer->source[lexer->position + 1] == 'b'))
    {
        int length = 2;

        while (isdigit((unsigned char)
                lexer->source[lexer->position + length]))
        {
            length++;
        }

        return make_token(lexer, TOKEN_ERROR, length);
    }

    // Identifiers and keywords.
    if (is_identifier_start(current))
    {
        return read_identifier(lexer);
    }

    // Numbers.
    if (isdigit((unsigned char)current))
    {
        return read_number(lexer);
    }

    // Malformed leading decimal point.
    if (current == '.' &&
        isdigit((unsigned char)lexer->source[lexer->position + 1]))
    {
        int length = 1;

        while (isdigit((unsigned char)lexer->source[lexer->position + length]) ||
               lexer->source[lexer->position + length] == '.')
        {
            length++;
        }

        while (is_identifier_part(lexer->source[lexer->position + length]))
        {
            length++;
        }

        return make_token(lexer, TOKEN_ERROR, length);
    }

    // Get the current context again.
    context = current_context(lexer);

    // Handle the end of an interpolation.
    if (current == '}' &&
        context != NULL &&
        context->type == LEXER_CONTEXT_INTERPOLATION)
    {
        if (context->brace_depth == 1)
        {
            Token token = make_token(
                lexer,
                TOKEN_INTERPOLATION_END,
                1
            );

            // Return to the string context below this interpolation.
            pop_context(lexer);

            return token;
        }

        // This is a regular closing brace inside the expression.
        context->brace_depth--;

        return read_operator_or_delimiter(lexer);
    }

    // Track nested braces inside the current interpolation.
    if (current == '{' &&
        context != NULL &&
        context->type == LEXER_CONTEXT_INTERPOLATION)
    {
        context->brace_depth++;
    }

    return read_operator_or_delimiter(lexer);
}