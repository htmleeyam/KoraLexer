
#include <stdio.h>
#include <string.h>
#include "../lexer.h"

// Test statistics.
static int checks_run = 0;
static int checks_failed = 0;

// Report the result of one test check.
static void check(int condition, const char *description) {
    checks_run++;

    if (condition) {
        printf("[PASS] %s\n", description);
    } else {
        printf("[FAIL] %s\n", description);
        checks_failed++;
    }
}

// Initialize a lexer for a source string.
static Lexer create_lexer(const char *source) {
    Lexer lexer = {0};

    lexer.source = source;
    lexer.line = 1;
    lexer.column = 1;

    return lexer;
}

// Compare a token's type and exact lexeme.
static int token_matches(
    const Token *token,
    TokenType expected_type,
    const char *expected_lexeme
) {
    size_t expected_length = strlen(expected_lexeme);

    return token->type == expected_type &&
           token->length == (int)expected_length &&
           token->lexeme != NULL &&
           (expected_length == 0 ||
            memcmp(token->lexeme, expected_lexeme, expected_length) == 0);
}

// Test 1: Basic token recognition and source locations.
static void test_basic_tokens(void) {
    printf("\n--- Test 1: Basic Token Recognition ---\n");

    const char source[] = "let count = 42";
    Lexer lexer = create_lexer(source);

    Token token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_KEYWORD, "let"),
        "Recognizes 'let' as KEYWORD"
    );

    check(
        token.line == 1 && token.column == 1,
        "Correct location for 'let'"
    );

    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_IDENTIFIER, "count"),
        "Recognizes 'count' as IDENTIFIER"
    );

    check(
        token.line == 1 && token.column == 5,
        "Correct location for 'count'"
    );

    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_ASSIGN, "="),
        "Recognizes assignment operator"
    );

    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_INTEGER, "42"),
        "Recognizes integer literal"
    );

    token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF && token.length == 0,
        "Correctly reaches EOF"
    );
}

// Test 2: Long identifier preservation.
static void test_long_identifier(void) {
    printf("\n--- Test 2: Long Identifier Preservation ---\n");

    char source[301];

    memset(source, 'a', 300);
    source[300] = '\0';

    Lexer lexer = create_lexer(source);

    Token token = get_next_token(&lexer);

    check(
        token.type == TOKEN_IDENTIFIER,
        "Recognizes long identifier"
    );

    check(
        token.length == 300,
        "Preserves exact 300-character length"
    );

    check(
        token.lexeme == source &&
        memcmp(token.lexeme, source, 300) == 0,
        "Preserves complete original source lexeme"
    );

    token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Correctly reaches EOF after long identifier"
    );
}

// Test 3: CRLF newline handling.
static void test_crlf_newline(void) {
    printf("\n--- Test 3: CRLF Newline Handling ---\n");

    const char source[] = "a\r\nb";
    Lexer lexer = create_lexer(source);

    Token token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_IDENTIFIER, "a"),
        "Recognizes identifier before CRLF"
    );

    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_NEWLINE, "\r\n"),
        "Recognizes CRLF as one NEWLINE token"
    );

    check(
        token.length == 2,
        "Preserves both CRLF source characters"
    );

    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_IDENTIFIER, "b"),
        "Recognizes identifier after CRLF"
    );

    check(
        token.line == 2 && token.column == 1,
        "Correctly updates line and column after CRLF"
    );
}


// Test 4: Reserved-word and identifier boundaries.
static void test_keyword_boundaries(void) {
    printf("\n--- Test 4: Keyword Boundary Recognition ---\n");

    const char source[] =
        "function fn current synch sync synchronize auto "
        "st oct bt octave semitone semitones constant "
        "forEach String string true false";

    static const char *expected_lexemes[] = {
        "function",
        "fn",
        "current",
        "synch",
        "sync",
        "synchronize",
        "auto",
        "st",
        "oct",
        "bt",
        "octave",
        "semitone",
        "semitones",
        "constant",
        "forEach",
        "String",
        "string",
        "true",
        "false"
    };

    static const TokenType expected_types[] = {
        TOKEN_KEYWORD,
        TOKEN_KEYWORD,
        TOKEN_KEYWORD,
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_KEYWORD,
        TOKEN_INTERVAL_UNIT,
        TOKEN_INTERVAL_UNIT,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER,
        TOKEN_TYPE,
        TOKEN_IDENTIFIER,
        TOKEN_BOOLEAN,
        TOKEN_BOOLEAN
    };

    Lexer lexer = create_lexer(source);

    size_t count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (size_t i = 0; i < count; i++) {
        Token token = get_next_token(&lexer);

        char description[128];

        snprintf(
            description,
            sizeof(description),
            "Correctly classifies '%s'",
            expected_lexemes[i]
        );

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            description
        );
    }

    Token token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after all reserved-word tests"
    );
}


// Test 5: Pitch literal recognition and boundaries.
static void test_pitch_boundaries(void) {
    printf("\n--- Test 5: Pitch Literal Boundaries ---\n");

    const char source[] =
        "C4 C-1 G9 C4Note C10 C#";

    static const char *expected_lexemes[] = {
        "C4",
        "C-1",
        "G9",
        "C4Note",
        "C10"
    };

    static const TokenType expected_types[] = {
        TOKEN_PITCH,
        TOKEN_PITCH,
        TOKEN_PITCH,
        TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER
    };

    Lexer lexer = create_lexer(source);

    size_t count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (size_t i = 0; i < count; i++) {
        Token token = get_next_token(&lexer);

        char description[128];

        snprintf(
            description,
            sizeof(description),
            "Correctly classifies '%s'",
            expected_lexemes[i]
        );

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            description
        );
    }

    // C# is an incomplete pitch literal.
    Token token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_ERROR, "C#"),
        "Detects incomplete pitch literal C#"
    );

    check(
        token.error_code == LEX_ERROR_INVALID_PITCH,
        "Uses the correct invalid-pitch error category"
    );

    check(
        token.line == 1 && token.column == 22,
        "Reports the correct location of malformed pitch"
    );

    token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after pitch tests"
    );
}


// Test 6: Numeric literal recognition and validation.
static void test_numeric_literals(void) {
    printf("\n--- Test 6: Numeric Literal Validation ---\n");

    const char source[] =
        "0 42 0.75 3.14 -42 12.3.4 123abc 12. .5";

    static const char *expected_lexemes[] = {
        "0",
        "42",
        "0.75",
        "3.14",
        "-",
        "42",
        "12.3.4",
        "123abc",
        "12",
        ".",
        ".5"
    };

    static const TokenType expected_types[] = {
        TOKEN_INTEGER,
        TOKEN_INTEGER,
        TOKEN_FLOAT,
        TOKEN_FLOAT,
        TOKEN_MINUS,
        TOKEN_INTEGER,
        TOKEN_ERROR,
        TOKEN_ERROR,
        TOKEN_INTEGER,
        TOKEN_DOT,
        TOKEN_ERROR
    };

    Lexer lexer = create_lexer(source);

    size_t count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (size_t i = 0; i < count; i++) {
        Token token = get_next_token(&lexer);

        char description[128];

        snprintf(
            description,
            sizeof(description),
            "Correctly recognizes numeric token '%s'",
            expected_lexemes[i]
        );

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            description
        );

        if (expected_types[i] == TOKEN_ERROR) {
            check(
                token.error_code == LEX_ERROR_INVALID_NUMBER,
                "Uses INVALID_NUMBER error category"
            );
        }
    }

    Token token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after numeric tests"
    );
}


// Test 7: Operators and delimiters.
static void test_operators_and_delimiters(void) {
    printf("\n--- Test 7: Operators and Delimiters ---\n");

    const char source[] =
        "+ - * / % ** ++ -- "
        "= += -= *= /= %= "
        "== != < > <= >= "
        "&& || ! |> -> => "
        "( ) { } [ ] , : . ;";

    static const char *expected_lexemes[] = {
        "+", "-", "*", "/", "%", "**",
        "++", "--",
        "=", "+=", "-=", "*=", "/=", "%=",
        "==", "!=", "<", ">", "<=", ">=",
        "&&", "||", "!",
        "|>", "->", "=>",
        "(", ")", "{", "}", "[", "]",
        ",", ":", ".", ";"
    };

    static const TokenType expected_types[] = {
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_MULTIPLY,
        TOKEN_DIVIDE,
        TOKEN_MODULO,
        TOKEN_POWER,

        TOKEN_INCREMENT,
        TOKEN_DECREMENT,

        TOKEN_ASSIGN,
        TOKEN_PLUS_ASSIGN,
        TOKEN_MINUS_ASSIGN,
        TOKEN_MULTIPLY_ASSIGN,
        TOKEN_DIVIDE_ASSIGN,
        TOKEN_MODULO_ASSIGN,

        TOKEN_EQUAL,
        TOKEN_NOT_EQUAL,
        TOKEN_LESS_THAN,
        TOKEN_GREATER_THAN,
        TOKEN_LESS_EQUAL,
        TOKEN_GREATER_EQUAL,

        TOKEN_AND,
        TOKEN_OR,
        TOKEN_NOT,

        TOKEN_PIPELINE,
        TOKEN_RETURN_ARROW,
        TOKEN_LAMBDA_ARROW,

        TOKEN_LEFT_PAREN,
        TOKEN_RIGHT_PAREN,
        TOKEN_LEFT_BRACE,
        TOKEN_RIGHT_BRACE,
        TOKEN_LEFT_BRACKET,
        TOKEN_RIGHT_BRACKET,

        TOKEN_COMMA,
        TOKEN_COLON,
        TOKEN_DOT,
        TOKEN_SEMICOLON
    };

    Lexer lexer = create_lexer(source);

    size_t count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (size_t i = 0; i < count; i++) {
        Token token = get_next_token(&lexer);

        char description[128];

        snprintf(
            description,
            sizeof(description),
            "Correctly recognizes operator/delimiter '%s'",
            expected_lexemes[i]
        );

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            description
        );
    }

    Token token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after operator and delimiter tests"
    );
}

void test_adjacent_operators(void) {
    printf("\n--- Test 8: Adjacent Operators and Longest Match ---\n");

    const char *source =
        "a++b a--b a**b "
        "a+=1 a-=1 a*=1 a/=1 a%=1 "
        "a==b a!=b a<=b a>=b "
        "a&&b a||b a|>b a->b a=>b";

    const TokenType expected_types[] = {
        TOKEN_IDENTIFIER, TOKEN_INCREMENT, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_DECREMENT, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_POWER, TOKEN_IDENTIFIER,

        TOKEN_IDENTIFIER, TOKEN_PLUS_ASSIGN, TOKEN_INTEGER,
        TOKEN_IDENTIFIER, TOKEN_MINUS_ASSIGN, TOKEN_INTEGER,
        TOKEN_IDENTIFIER, TOKEN_MULTIPLY_ASSIGN, TOKEN_INTEGER,
        TOKEN_IDENTIFIER, TOKEN_DIVIDE_ASSIGN, TOKEN_INTEGER,
        TOKEN_IDENTIFIER, TOKEN_MODULO_ASSIGN, TOKEN_INTEGER,

        TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_NOT_EQUAL, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_LESS_EQUAL, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_GREATER_EQUAL, TOKEN_IDENTIFIER,

        TOKEN_IDENTIFIER, TOKEN_AND, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_OR, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_PIPELINE, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_RETURN_ARROW, TOKEN_IDENTIFIER,
        TOKEN_IDENTIFIER, TOKEN_LAMBDA_ARROW, TOKEN_IDENTIFIER
    };

    const char *expected_lexemes[] = {
        "a", "++", "b",
        "a", "--", "b",
        "a", "**", "b",

        "a", "+=", "1",
        "a", "-=", "1",
        "a", "*=", "1",
        "a", "/=", "1",
        "a", "%=", "1",

        "a", "==", "b",
        "a", "!=", "b",
        "a", "<=", "b",
        "a", ">=", "b",

        "a", "&&", "b",
        "a", "||", "b",
        "a", "|>", "b",
        "a", "->", "b",
        "a", "=>", "b"
    };

    Lexer lexer = create_lexer(source);

    int expected_count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (int i = 0; i < expected_count; i++) {
        Token token = get_next_token(&lexer);

        check(
            token_matches(&token, expected_types[i], expected_lexemes[i]),
            "Correctly recognizes adjacent operator using longest match"
        );
    }

    Token eof = get_next_token(&lexer);
    check(eof.type == TOKEN_EOF,
          "Correctly reaches EOF after adjacent operator tests");
}

void test_string_literals_and_escapes(void) {
    printf("\n--- Test 9: String Literals and Escape Sequences ---\n");

    const char *source =
        "\"hello\" \"\" \"line\\n\\t\\r\\\\\\\"\\{\\'end\"";

    Lexer lexer = create_lexer(source);

    Token token;

    // Ordinary string: "hello"
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes ordinary string start");

    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello"),
          "Preserves ordinary string content");

    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_END, "\""),
          "Recognizes ordinary string end");

    // Empty string: ""
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes empty string start");

    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_END, "\""),
          "Recognizes empty string end without string content");

    // String containing all seven supported escape sequences.
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes escaped string start");

    token = get_next_token(&lexer);
    check(
        token_matches(
            &token,
            TOKEN_STRING_TEXT,
            "line\\n\\t\\r\\\\\\\"\\{\\'end"
        ),
        "Preserves all seven supported escape sequences"
    );

    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_STRING_END, "\""),
          "Recognizes escaped string end");

    token = get_next_token(&lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after valid string tests");

    // Invalid escape sequence: \q
    const char *invalid_source = "\"valid\\q\"";
    Lexer invalid_lexer = create_lexer(invalid_source);

    token = get_next_token(&invalid_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before invalid escape");

    token = get_next_token(&invalid_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "valid"),
          "Preserves valid text before invalid escape");

    token = get_next_token(&invalid_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\\q"),
          "Detects invalid escape sequence");

    check(token.error_code == LEX_ERROR_INVALID_ESCAPE,
          "Uses INVALID_ESCAPE error category");

    token = get_next_token(&invalid_lexer);
    check(token_matches(&token, TOKEN_STRING_END, "\""),
          "Continues scanning after invalid escape");

    token = get_next_token(&invalid_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after invalid escape test");
}

void test_basic_interpolation(void) {
    printf("\n--- Test 10: Basic String Interpolation ---\n");

    const char *source =
        "\"Hello {name}!\" "
        "\"Result: {score + 5}\"";

    const TokenType expected_types[] = {
        // First string: "Hello {name}!"
        TOKEN_STRING_START,
        TOKEN_STRING_TEXT,
        TOKEN_INTERPOLATION_START,
        TOKEN_IDENTIFIER,
        TOKEN_INTERPOLATION_END,
        TOKEN_STRING_TEXT,
        TOKEN_STRING_END,

        // Second string: "Result: {score + 5}"
        TOKEN_STRING_START,
        TOKEN_STRING_TEXT,
        TOKEN_INTERPOLATION_START,
        TOKEN_IDENTIFIER,
        TOKEN_PLUS,
        TOKEN_INTEGER,
        TOKEN_INTERPOLATION_END,
        TOKEN_STRING_END
    };

    const char *expected_lexemes[] = {
        "\"",
        "Hello ",
        "{",
        "name",
        "}",
        "!",
        "\"",

        "\"",
        "Result: ",
        "{",
        "score",
        "+",
        "5",
        "}",
        "\""
    };

    Lexer lexer = create_lexer(source);

    int expected_count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (int i = 0; i < expected_count; i++) {
        Token token = get_next_token(&lexer);

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            "Correctly recognizes interpolation token and lexeme"
        );
    }

    Token eof = get_next_token(&lexer);

    check(
        eof.type == TOKEN_EOF,
        "Reaches EOF after interpolation tests"
    );
}

void test_nested_interpolation(void) {
    printf("\n--- Test 11: Nested Braces and Nested Strings ---\n");

    const char *source =
        "\"Object: {{key: value}}!\" "
        "\"Nested: {\"inner {name}\"} tail\"";

    const TokenType expected_types[] = {
        // First string: "Object: {{key: value}}!"
        TOKEN_STRING_START,
        TOKEN_STRING_TEXT,
        TOKEN_INTERPOLATION_START,
        TOKEN_LEFT_BRACE,
        TOKEN_IDENTIFIER,
        TOKEN_COLON,
        TOKEN_IDENTIFIER,
        TOKEN_RIGHT_BRACE,
        TOKEN_INTERPOLATION_END,
        TOKEN_STRING_TEXT,
        TOKEN_STRING_END,

        // Second string: "Nested: {"inner {name}"} tail"
        TOKEN_STRING_START,
        TOKEN_STRING_TEXT,
        TOKEN_INTERPOLATION_START,
        TOKEN_STRING_START,
        TOKEN_STRING_TEXT,
        TOKEN_INTERPOLATION_START,
        TOKEN_IDENTIFIER,
        TOKEN_INTERPOLATION_END,
        TOKEN_STRING_END,
        TOKEN_INTERPOLATION_END,
        TOKEN_STRING_TEXT,
        TOKEN_STRING_END
    };

    const char *expected_lexemes[] = {
        "\"",
        "Object: ",
        "{",
        "{",
        "key",
        ":",
        "value",
        "}",
        "}",
        "!",
        "\"",

        "\"",
        "Nested: ",
        "{",
        "\"",
        "inner ",
        "{",
        "name",
        "}",
        "\"",
        "}",
        " tail",
        "\""
    };

    Lexer lexer = create_lexer(source);

    int expected_count =
        sizeof(expected_types) / sizeof(expected_types[0]);

    for (int i = 0; i < expected_count; i++) {
        Token token = get_next_token(&lexer);

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            "Correctly recognizes nested interpolation token"
        );
    }

    Token eof = get_next_token(&lexer);

    check(
        eof.type == TOKEN_EOF,
        "Reaches EOF after nested interpolation tests"
    );
}

void test_unterminated_strings(void) {
    printf("\n--- Test 12: Unterminated Strings and Recovery ---\n");

    Token token;

    // Case 1: Physical LF newline inside a string.
    const char *lf_source = "\"unfinished\nnext";
    Lexer lf_lexer = create_lexer(lf_source);

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "unfinished"),
          "Preserves text before LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\n"),
          "Detects physical LF inside string");

    check(token.error_code == LEX_ERROR_UNTERMINATED_STRING,
          "Uses UNTERMINATED_STRING error category for LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "next"),
          "Recovers and recognizes identifier after LF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after LF recovery");

    token = get_next_token(&lf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after LF recovery");


    // Case 2: String reaches EOF without a closing quotation mark.
    const char *eof_source = "\"unfinished";
    Lexer eof_lexer = create_lexer(eof_source);

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before EOF");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "unfinished"),
          "Preserves text before EOF");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_ERROR, ""),
          "Detects unterminated string at EOF");

    check(token.error_code == LEX_ERROR_UNTERMINATED_STRING,
          "Uses UNTERMINATED_STRING error category at EOF");

    token = get_next_token(&eof_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after unterminated string error");


    // Case 3: CRLF newline inside a string.
    const char *crlf_source = "\"oops\r\nnext";
    Lexer crlf_lexer = create_lexer(crlf_source);

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "oops"),
          "Preserves text before CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r\n"),
          "Detects CRLF inside string as one error token");

    check(token.error_code == LEX_ERROR_UNTERMINATED_STRING,
          "Uses UNTERMINATED_STRING error category for CRLF");

    check(token.line == 1 && token.column == 6,
          "Reports correct location of CRLF error");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "next"),
          "Recovers and recognizes identifier after CRLF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after CRLF recovery");

    token = get_next_token(&crlf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after CRLF recovery");
}

void test_unterminated_interpolation(void) {
    printf("\n--- Test 13: Unterminated Interpolation and Recovery ---\n");

    Token token;

    // Case 1: Interpolation reaches EOF without a closing brace.
    const char *eof_source = "\"Hello {name";
    Lexer eof_lexer = create_lexer(eof_source);

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before unfinished interpolation");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "Hello "),
          "Preserves string text before interpolation");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "name"),
          "Recognizes expression inside unfinished interpolation");

    token = get_next_token(&eof_lexer);
    check(token_matches(&token, TOKEN_ERROR, ""),
          "Detects unfinished interpolation at EOF");

    check(token.error_code == LEX_ERROR_UNTERMINATED_INTERPOLATION,
          "Uses UNTERMINATED_INTERPOLATION error category at EOF");

    token = get_next_token(&eof_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after unterminated interpolation error");


    // Case 2: LF newline inside interpolation.
    const char *lf_source = "\"Hello {name\nnext";
    Lexer lf_lexer = create_lexer(lf_source);

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before interpolation LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "Hello "),
          "Preserves text before interpolation LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start before LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "name"),
          "Recognizes identifier before interpolation LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\n"),
          "Detects LF inside interpolation");

    check(token.error_code == LEX_ERROR_UNTERMINATED_INTERPOLATION,
          "Uses UNTERMINATED_INTERPOLATION error category for LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "next"),
          "Recovers and recognizes identifier after interpolation LF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after interpolation LF");

    token = get_next_token(&lf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after LF interpolation recovery");


    // Case 3: CRLF newline inside interpolation.
    const char *crlf_source = "\"Hello {name\r\nnext";
    Lexer crlf_lexer = create_lexer(crlf_source);

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before interpolation CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "Hello "),
          "Preserves text before interpolation CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start before CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "name"),
          "Recognizes identifier before interpolation CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r\n"),
          "Detects CRLF inside interpolation as one error token");

    check(token.error_code == LEX_ERROR_UNTERMINATED_INTERPOLATION,
          "Uses UNTERMINATED_INTERPOLATION error category for CRLF");

    check(token.line == 1 && token.column == 13,
          "Reports correct location of interpolation CRLF error");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "next"),
          "Recovers and recognizes identifier after interpolation CRLF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after interpolation CRLF");

    token = get_next_token(&crlf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after CRLF interpolation recovery");
}

void test_comments(void) {
    printf("\n--- Test 14: Single-Line and Block Comments ---\n");

    Token token;

    // Case 1: Single-line comment.
    const char *single_source = "alpha~~ comment\nbeta";
    Lexer single_lexer = create_lexer(single_source);

    token = get_next_token(&single_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
          "Recognizes identifier before single-line comment");

    token = get_next_token(&single_lexer);
    check(token_matches(&token, TOKEN_NEWLINE, "\n"),
          "Preserves newline after single-line comment");

    check(token.line == 1 && token.column == 16,
          "Reports correct newline location after single-line comment");

    token = get_next_token(&single_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "beta"),
          "Recognizes identifier after single-line comment");

    token = get_next_token(&single_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after single-line comment");


    // Case 2: Block comment on one line.
    const char *block_source = "alpha ~* hidden text *~ beta";
    Lexer block_lexer = create_lexer(block_source);

    token = get_next_token(&block_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
          "Recognizes identifier before block comment");

    token = get_next_token(&block_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "beta"),
          "Skips block comment and recognizes following identifier");

    token = get_next_token(&block_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after single-line block comment");


    // Case 3: Multiline block comment.
    const char *multiline_source =
        "alpha~*first\nsecond\nthird*~beta";

    Lexer multiline_lexer = create_lexer(multiline_source);

    token = get_next_token(&multiline_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
          "Recognizes identifier before multiline block comment");

    token = get_next_token(&multiline_lexer);
    check(token_matches(&token, TOKEN_NEWLINE, "\n"),
          "Emits first newline inside block comment");

    check(token.line == 1 && token.column == 13,
          "Reports correct location of first block-comment newline");

    token = get_next_token(&multiline_lexer);
    check(token_matches(&token, TOKEN_NEWLINE, "\n"),
          "Emits second newline inside block comment");

    check(token.line == 2 && token.column == 7,
          "Reports correct location of second block-comment newline");

    token = get_next_token(&multiline_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "beta"),
          "Recognizes identifier after multiline block comment");

    check(token.line == 3 && token.column == 8,
          "Correctly updates location after multiline block comment");

    token = get_next_token(&multiline_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after multiline block comment");


    // Case 4: Unterminated block comment.
    const char *unterminated_source = "alpha\n~* unfinished";
    Lexer unterminated_lexer = create_lexer(unterminated_source);

    token = get_next_token(&unterminated_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
          "Recognizes identifier before unterminated block comment");

    token = get_next_token(&unterminated_lexer);
    check(token_matches(&token, TOKEN_NEWLINE, "\n"),
          "Recognizes newline before unterminated block comment");

    token = get_next_token(&unterminated_lexer);
    check(token_matches(&token, TOKEN_ERROR, "~*"),
          "Detects unterminated block comment");

    check(token.error_code == LEX_ERROR_UNTERMINATED_BLOCK_COMMENT,
          "Uses UNTERMINATED_BLOCK_COMMENT error category");

    check(token.line == 2 && token.column == 1,
          "Reports the opening position of unterminated block comment");

    token = get_next_token(&unterminated_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after unterminated block comment error");
}

void test_context_stack_limit(void) {
    printf("\n--- Test 15: Lexer Context Stack Limit ---\n");

    char source[(MAX_LEXER_CONTEXTS * 2) + 6];
    int position = 0;

    // Fill all available contexts by alternating strings and interpolation.
    for (int i = 0; i < MAX_LEXER_CONTEXTS + 1; i++) {
        source[position++] = (i % 2 == 0) ? '"' : '{';
    }

    // Add closing delimiters to unwind the contexts after the error.
    for (int i = 0; i < MAX_LEXER_CONTEXTS; i++) {
        source[position++] = (i % 2 == 0) ? '}' : '"';
    }

    // Add a valid identifier to verify recovery.
    memcpy(source + position, "tail", sizeof("tail"));

    Lexer lexer = create_lexer(source);

    Token token;

    // Verify all 64 context pushes.
    for (int i = 0; i < MAX_LEXER_CONTEXTS; i++) {
        token = get_next_token(&lexer);

        if (i % 2 == 0) {
            check(
                token_matches(&token, TOKEN_STRING_START, "\""),
                "Successfully pushes string context"
            );
        } else {
            check(
                token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
                "Successfully pushes interpolation context"
            );
        }
    }

    // The 65th context should exceed the limit.
    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_ERROR, "\""),
        "Detects context stack limit overflow"
    );

    check(
        token.error_code == LEX_ERROR_CONTEXT_LIMIT,
        "Uses CONTEXT_LIMIT error category"
    );

    // Verify that existing contexts can still be closed.
    for (int i = 0; i < MAX_LEXER_CONTEXTS; i++) {
        token = get_next_token(&lexer);

        if (i % 2 == 0) {
            check(
                token_matches(&token, TOKEN_INTERPOLATION_END, "}"),
                "Successfully closes interpolation context"
            );
        } else {
            check(
                token_matches(&token, TOKEN_STRING_END, "\""),
                "Successfully closes string context"
            );
        }
    }

    // Verify scanning continues after the contexts are closed.
    token = get_next_token(&lexer);

    check(
        token_matches(&token, TOKEN_IDENTIFIER, "tail"),
        "Recognizes identifier after context stack recovery"
    );

    token = get_next_token(&lexer);

    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after context stack recovery"
    );
}

void test_standalone_carriage_return(void) {
    printf("\n--- Test 16: Standalone Carriage Return ---\n");

    const char *source = "alpha\rbeta";
    Lexer lexer = create_lexer(source);

    Token token;

    // Identifier before standalone CR.
    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
        "Recognizes identifier before standalone CR"
    );

    // Standalone CR must be rejected.
    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_ERROR, "\r"),
        "Detects standalone CR as a lexical error"
    );

    check(
        token.error_code == LEX_ERROR_INVALID_NEWLINE,
        "Uses INVALID_NEWLINE error category"
    );

    check(
        token.line == 1 && token.column == 6,
        "Reports correct location of standalone CR"
    );

    // Scanning should continue after the invalid character.
    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "beta"),
        "Recovers and recognizes identifier after standalone CR"
    );

    check(
        token.line == 1 && token.column == 7,
        "Does not incorrectly increment line number after standalone CR"
    );

    token = get_next_token(&lexer);
    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after standalone CR recovery"
    );
}

void test_complete_reserved_word_inventory(void) {
    printf("\n--- Test 17: Complete Reserved-Word Inventory ---\n");

    const char *source =
        "class struct track verse new "
        "const let auto function fn pure return "
        "tempo volume beat add synch "
        "if else for while break "
        "try catch throw current "
        "true false "
        "int float boolean String pitch interval event pattern "
        "st oct";

    const char *expected_lexemes[] = {
        "class", "struct", "track", "verse", "new",
        "const", "let", "auto", "function", "fn", "pure", "return",
        "tempo", "volume", "beat", "add", "synch",
        "if", "else", "for", "while", "break",
        "try", "catch", "throw", "current",
        "true", "false",
        "int", "float", "boolean", "String",
        "pitch", "interval", "event", "pattern",
        "st", "oct"
    };

    const TokenType expected_types[] = {
        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD, TOKEN_KEYWORD,

        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD,

        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD, TOKEN_KEYWORD,

        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD, TOKEN_KEYWORD,

        TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
        TOKEN_KEYWORD,

        TOKEN_BOOLEAN, TOKEN_BOOLEAN,

        TOKEN_TYPE, TOKEN_TYPE, TOKEN_TYPE, TOKEN_TYPE,
        TOKEN_TYPE, TOKEN_TYPE, TOKEN_TYPE, TOKEN_TYPE,

        TOKEN_INTERVAL_UNIT, TOKEN_INTERVAL_UNIT
    };

    Lexer lexer = create_lexer(source);

    int count = sizeof(expected_types) / sizeof(expected_types[0]);

    check(count == 38,
          "Test contains all 38 reserved lexemes");

    for (int i = 0; i < count; i++) {
        Token token = get_next_token(&lexer);

        check(
            token_matches(
                &token,
                expected_types[i],
                expected_lexemes[i]
            ),
            "Correctly classifies reserved lexeme"
        );
    }

    Token eof = get_next_token(&lexer);

    check(
        eof.type == TOKEN_EOF,
        "Reaches EOF after all 38 reserved lexemes"
    );
}

void test_unexpected_characters(void) {
    printf("\n--- Test 18: Unexpected Characters and Error Recovery ---\n");

    const char *source = "alpha @ beta ? gamma & delta | omega";
    Lexer lexer = create_lexer(source);

    Token token;

    // Identifier before first invalid character.
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
          "Recognizes identifier before unexpected character");

    // Unexpected @
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_ERROR, "@"),
          "Detects unexpected @ character");

    check(token.error_code == LEX_ERROR_UNEXPECTED_CHARACTER,
          "Uses UNEXPECTED_CHARACTER error category for @");

    check(token.line == 1 && token.column == 7,
          "Reports correct location of @");

    // Identifier after @
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "beta"),
          "Continues scanning after @ error");

    // Unexpected ?
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_ERROR, "?"),
          "Detects unexpected ? character");

    check(token.error_code == LEX_ERROR_UNEXPECTED_CHARACTER,
          "Uses UNEXPECTED_CHARACTER error category for ?");

    check(token.line == 1 && token.column == 14,
          "Reports correct location of ?");

    // Identifier after ?
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "gamma"),
          "Continues scanning after ? error");

    // Unexpected single &
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_ERROR, "&"),
          "Rejects standalone & character");

    check(token.error_code == LEX_ERROR_UNEXPECTED_CHARACTER,
          "Uses UNEXPECTED_CHARACTER error category for standalone &");

    check(token.line == 1 && token.column == 22,
          "Reports correct location of standalone &");

    // Identifier after &
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "delta"),
          "Continues scanning after standalone & error");

    // Unexpected single |
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_ERROR, "|"),
          "Rejects standalone | character");

    check(token.error_code == LEX_ERROR_UNEXPECTED_CHARACTER,
          "Uses UNEXPECTED_CHARACTER error category for standalone |");

    check(token.line == 1 && token.column == 30,
          "Reports correct location of standalone |");

    // Identifier after |
    token = get_next_token(&lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "omega"),
          "Continues scanning after standalone | error");

    token = get_next_token(&lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after unexpected-character recovery");
}

void test_escaped_opening_brace(void) {
    printf("\n--- Test 19: Escaped Opening Brace ---\n");

    const char *source = "\"Literal \\{name}\"";
    Lexer lexer = create_lexer(source);

    Token token;

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_STRING_START, "\""),
        "Recognizes string start"
    );

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_STRING_TEXT, "Literal \\{name}"),
        "Preserves escaped opening brace as string content"
    );

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_STRING_END, "\""),
        "Recognizes closing quotation mark"
    );

    token = get_next_token(&lexer);
    check(
        token.type == TOKEN_EOF,
        "Reaches EOF without starting interpolation"
    );
}

void test_dangling_backslash(void) {
    printf("\n--- Test 20: Dangling Backslash at End of String ---\n");

    // Source contains an opening quote, text, and a trailing backslash.
    // There is deliberately no closing quotation mark.
    const char *source = "\"dangling\\";
    Lexer lexer = create_lexer(source);

    Token token;

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_STRING_START, "\""),
        "Recognizes string start"
    );

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_STRING_TEXT, "dangling"),
        "Preserves string text before dangling backslash"
    );

    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_ERROR, "\\"),
        "Detects dangling backslash as invalid escape"
    );

    check(
        token.error_code == LEX_ERROR_INVALID_ESCAPE,
        "Uses INVALID_ESCAPE error category"
    );

    // The string context remains open, so EOF should also report
    // that the closing quotation mark is missing.
    token = get_next_token(&lexer);
    check(
        token_matches(&token, TOKEN_ERROR, ""),
        "Detects unterminated string after invalid escape"
    );

    check(
        token.error_code == LEX_ERROR_UNTERMINATED_STRING,
        "Uses UNTERMINATED_STRING error category"
    );

    token = get_next_token(&lexer);
    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after dangling backslash errors"
    );
}



void test_accidental_pitch_boundaries(void) {
    printf("\n--- Test 21: Accidental Pitch Boundaries ---\n");

    const struct {
        const char *source;
        TokenType expected_type;
        const char *description;
    } cases[] = {
        {"F#3", TOKEN_PITCH, "Recognizes sharp pitch F#3"},
        {"Bb3", TOKEN_PITCH, "Recognizes flat pitch Bb3"},
        {"C#-1", TOKEN_PITCH, "Recognizes accidental pitch C#-1"},
        {"F3Note", TOKEN_IDENTIFIER,
         "Keeps F3Note as one identifier"},
        {"H4", TOKEN_IDENTIFIER,
         "Keeps non-pitch note H4 as an identifier"}
    };

    int count = sizeof(cases) / sizeof(cases[0]);

    for (int i = 0; i < count; i++) {
        Lexer lexer = create_lexer(cases[i].source);

        Token token = get_next_token(&lexer);

        check(
            token_matches(
                &token,
                cases[i].expected_type,
                cases[i].source
            ),
            cases[i].description
        );

        Token eof = get_next_token(&lexer);

        check(
            eof.type == TOKEN_EOF,
            "Reaches EOF after pitch boundary test"
        );
    }

    // Accidental pitch followed by an identifier suffix.
    const char *malformed_sources[] = {
        "F#3Note",
        "Bb3Melody"
    };

    const char *pitch_prefixes[] = {
        "F#3",
        "Bb3"
    };

    const char *identifier_suffixes[] = {
        "Note",
        "Melody"
    };

    for (int i = 0; i < 2; i++) {
        Lexer lexer = create_lexer(malformed_sources[i]);

        Token token = get_next_token(&lexer);

        check(
            token_matches(&token, TOKEN_ERROR, pitch_prefixes[i]),
            "Rejects accidental pitch without a valid boundary"
        );

        check(
            token.error_code == LEX_ERROR_INVALID_PITCH,
            "Uses INVALID_PITCH error category"
        );

        token = get_next_token(&lexer);

        check(
            token_matches(
                &token,
                TOKEN_IDENTIFIER,
                identifier_suffixes[i]
            ),
            "Preserves identifier suffix after malformed pitch"
        );

        token = get_next_token(&lexer);

        check(
            token.type == TOKEN_EOF,
            "Reaches EOF after malformed pitch boundary test"
        );
    }
}

void test_comment_newline_edge_cases(void) {
    printf("\n--- Test 22: Newline Edge Cases Inside Block Comments ---\n");

    Token token;

    // Case 1: CRLF inside a block comment.
    const char *crlf_source =
        "alpha~*first\r\nsecond*~beta";

    Lexer crlf_lexer = create_lexer(crlf_source);

    token = get_next_token(&crlf_lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
        "Recognizes identifier before CRLF block comment"
    );

    token = get_next_token(&crlf_lexer);
    check(
        token_matches(&token, TOKEN_NEWLINE, "\r\n"),
        "Preserves CRLF as one newline token inside block comment"
    );

    check(
        token.line == 1 && token.column == 13,
        "Reports correct CRLF location inside block comment"
    );

    token = get_next_token(&crlf_lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "beta"),
        "Recognizes identifier after CRLF block comment"
    );

    check(
        token.line == 2 && token.column == 9,
        "Correctly updates location after CRLF block comment"
    );

    token = get_next_token(&crlf_lexer);
    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after CRLF block comment"
    );


    // Case 2: Standalone CR inside a block comment.
    const char *cr_source = "alpha~*x\ry*~beta";

    Lexer cr_lexer = create_lexer(cr_source);

    token = get_next_token(&cr_lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "alpha"),
        "Recognizes identifier before standalone CR in comment"
    );

    token = get_next_token(&cr_lexer);
    check(
        token_matches(&token, TOKEN_ERROR, "\r"),
        "Rejects standalone CR inside block comment"
    );

    check(
        token.error_code == LEX_ERROR_INVALID_NEWLINE,
        "Uses INVALID_NEWLINE error category inside block comment"
    );

    check(
        token.line == 1 && token.column == 9,
        "Reports correct location of standalone CR in comment"
    );

    token = get_next_token(&cr_lexer);
    check(
        token_matches(&token, TOKEN_IDENTIFIER, "beta"),
        "Continues scanning after invalid CR inside block comment"
    );

    check(
        token.line == 1 && token.column == 13,
        "Does not increment line after standalone CR in comment"
    );

    token = get_next_token(&cr_lexer);
    check(
        token.type == TOKEN_EOF,
        "Reaches EOF after standalone CR comment recovery"
    );
}

void test_empty_and_whitespace_input(void) {
    printf("\n--- Test 23: Empty and Whitespace-Only Input ---\n");

    // Case 1: Completely empty source.
    Lexer empty_lexer = create_lexer("");

    Token token = get_next_token(&empty_lexer);

    check(
        token.type == TOKEN_EOF,
        "Immediately reaches EOF for empty source"
    );

    // Case 2: Source containing only spaces and tabs.
    Lexer whitespace_lexer = create_lexer("   \t\t   ");

    token = get_next_token(&whitespace_lexer);

    check(
        token.type == TOKEN_EOF,
        "Skips whitespace and reaches EOF without producing tokens"
    );
}

void test_block_comment_newline_inside_interpolation(void) {
    printf("\n--- Test 24: Block Comment Newlines Inside Interpolation ---\n");

    Token token;

    // Case 1: LF inside a block comment within interpolation.
    const char *lf_source =
        "\"x {value ~* comment\ncontinued *~";

    Lexer lf_lexer = create_lexer(lf_source);

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before LF block comment");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "x "),
          "Preserves string text before interpolation");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start before LF block comment");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "value"),
          "Recognizes identifier before LF block comment");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\n"),
          "Reports LF inside interpolation comment as an error");

    check(token.error_code == LEX_ERROR_UNTERMINATED_INTERPOLATION,
          "Uses UNTERMINATED_INTERPOLATION error category for LF");

    check(token.line == 1 && token.column == 21,
          "Reports correct location of LF inside interpolation comment");

    token = get_next_token(&lf_lexer);
    check(token.type == TOKEN_EOF,
          "Skips remaining LF block comment and reaches EOF");


    // Case 2: CRLF inside a block comment within interpolation.
    const char *crlf_source =
        "\"x {value ~* comment\r\ncontinued *~";

    Lexer crlf_lexer = create_lexer(crlf_source);

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before CRLF block comment");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "x "),
          "Preserves string text before CRLF interpolation");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start before CRLF block comment");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "value"),
          "Recognizes identifier before CRLF block comment");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r\n"),
          "Reports CRLF inside interpolation comment as an error");

    check(token.error_code == LEX_ERROR_UNTERMINATED_INTERPOLATION,
          "Uses UNTERMINATED_INTERPOLATION error category for CRLF");

    check(token.line == 1 && token.column == 21,
          "Reports correct location of CRLF inside interpolation comment");

    token = get_next_token(&crlf_lexer);
    check(token.type == TOKEN_EOF,
          "Skips remaining CRLF block comment and reaches EOF");


    // Case 3: Standalone CR inside a block comment within interpolation.
    const char *cr_source =
        "\"x {value ~* comment\rcontinued *~";

    Lexer cr_lexer = create_lexer(cr_source);

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before standalone CR comment");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "x "),
          "Preserves string text before standalone CR interpolation");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start before standalone CR comment");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "value"),
          "Recognizes identifier before standalone CR comment");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r"),
          "Detects standalone CR inside interpolation comment");

    check(token.error_code == LEX_ERROR_INVALID_NEWLINE,
          "Preserves INVALID_NEWLINE classification for standalone CR");

    check(token.line == 1 && token.column == 21,
          "Reports correct location of standalone CR in interpolation comment");

    token = get_next_token(&cr_lexer);
    check(token.type == TOKEN_EOF,
          "Ends interpolation and skips remaining comment after standalone CR");
}


void test_standalone_cr_inside_string_contexts(void) {
    printf("\n--- Test 25: Standalone CR Inside Strings and Interpolation ---\n");

    Token token;

    // Case 1: Standalone CR inside an ordinary string.
    const char *string_source = "\"hello\rworld";

    Lexer string_lexer = create_lexer(string_source);

    token = get_next_token(&string_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before standalone CR");

    token = get_next_token(&string_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello"),
          "Preserves string text before standalone CR");

    token = get_next_token(&string_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r"),
          "Detects standalone CR inside ordinary string");

    check(token.error_code == LEX_ERROR_INVALID_NEWLINE,
          "Uses INVALID_NEWLINE error category inside ordinary string");

    check(token.line == 1 && token.column == 7,
          "Reports correct location of standalone CR inside string");

    token = get_next_token(&string_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "world"),
          "Terminates string context and recognizes following identifier");

    token = get_next_token(&string_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after standalone CR inside string");


    // Case 2: Standalone CR inside an interpolation expression.
    const char *interpolation_source = "\"hello {1\r+ 2";

    Lexer interpolation_lexer = create_lexer(interpolation_source);

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before interpolation CR");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello "),
          "Preserves string text before interpolation");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_INTERPOLATION_START, "{"),
          "Recognizes interpolation start");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_INTEGER, "1"),
          "Recognizes integer before standalone CR in interpolation");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r"),
          "Detects standalone CR inside interpolation");

    check(token.error_code == LEX_ERROR_INVALID_NEWLINE,
          "Uses INVALID_NEWLINE error category inside interpolation");

    check(token.line == 1 && token.column == 10,
          "Reports correct location of standalone CR inside interpolation");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_PLUS, "+"),
          "Terminates string and interpolation contexts after standalone CR");

    token = get_next_token(&interpolation_lexer);
    check(token_matches(&token, TOKEN_INTEGER, "2"),
          "Continues recognizing tokens after interpolation recovery");

    token = get_next_token(&interpolation_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after standalone CR inside interpolation");
}


void test_backslash_before_physical_newlines(void) {
    printf("\n--- Test 26: Backslash Before Physical Newlines ---\n");

    Token token;

    // Case 1: Backslash followed by LF.
    const char *lf_source = "\"hello\\\nworld";
    Lexer lf_lexer = create_lexer(lf_source);

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before backslash and LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello"),
          "Preserves string text before backslash and LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\\"),
          "Reports backslash before LF as invalid escape");

    check(token.error_code == LEX_ERROR_INVALID_ESCAPE,
          "Uses INVALID_ESCAPE for backslash before LF");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\n"),
          "Processes LF separately after invalid escape");

    check(token.error_code == LEX_ERROR_UNTERMINATED_STRING,
          "Uses UNTERMINATED_STRING for LF inside string");

    check(token.line == 1 && token.column == 8,
          "Reports correct LF location after backslash");

    token = get_next_token(&lf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "world"),
          "Recovers and recognizes identifier after LF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after LF recovery");

    token = get_next_token(&lf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after LF recovery");


    // Case 2: Backslash followed by CRLF.
    const char *crlf_source = "\"hello\\\r\nworld";
    Lexer crlf_lexer = create_lexer(crlf_source);

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before backslash and CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello"),
          "Preserves string text before backslash and CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\\"),
          "Reports backslash before CRLF as invalid escape");

    check(token.error_code == LEX_ERROR_INVALID_ESCAPE,
          "Uses INVALID_ESCAPE for backslash before CRLF");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r\n"),
          "Processes CRLF separately after invalid escape");

    check(token.error_code == LEX_ERROR_UNTERMINATED_STRING,
          "Uses UNTERMINATED_STRING for CRLF inside string");

    check(token.line == 1 && token.column == 8,
          "Reports correct CRLF location after backslash");

    token = get_next_token(&crlf_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "world"),
          "Recovers and recognizes identifier after CRLF");

    check(token.line == 2 && token.column == 1,
          "Correctly updates location after CRLF recovery");

    token = get_next_token(&crlf_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after CRLF recovery");


    // Case 3: Backslash followed by standalone CR.
    const char *cr_source = "\"hello\\\rworld";
    Lexer cr_lexer = create_lexer(cr_source);

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_STRING_START, "\""),
          "Recognizes string start before backslash and standalone CR");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_STRING_TEXT, "hello"),
          "Preserves string text before standalone CR");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\\"),
          "Reports backslash before standalone CR as invalid escape");

    check(token.error_code == LEX_ERROR_INVALID_ESCAPE,
          "Uses INVALID_ESCAPE for backslash before standalone CR");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_ERROR, "\r"),
          "Processes standalone CR separately after invalid escape");

    check(token.error_code == LEX_ERROR_INVALID_NEWLINE,
          "Preserves INVALID_NEWLINE for standalone CR");

    check(token.line == 1 && token.column == 8,
          "Reports correct standalone CR location");

    token = get_next_token(&cr_lexer);
    check(token_matches(&token, TOKEN_IDENTIFIER, "world"),
          "Recovers and recognizes identifier after standalone CR");

    check(token.line == 1 && token.column == 9,
          "Does not incorrectly increment line after standalone CR");

    token = get_next_token(&cr_lexer);
    check(token.type == TOKEN_EOF,
          "Reaches EOF after standalone CR recovery");
}


int main(void) {
    printf("========================================\n");
    printf("       KORA LEXER AUTOMATED TESTS       \n");
    printf("========================================\n");

    test_basic_tokens();
    test_long_identifier();
    test_crlf_newline();
    test_keyword_boundaries();
    test_pitch_boundaries();
    test_numeric_literals();
    test_operators_and_delimiters();
    test_adjacent_operators();
    test_string_literals_and_escapes();
    test_basic_interpolation();
    test_nested_interpolation();
    test_unterminated_strings();
    test_unterminated_interpolation();
    test_comments();
    test_context_stack_limit();
    test_standalone_carriage_return();
    test_complete_reserved_word_inventory();
    test_unexpected_characters();
    test_escaped_opening_brace();
    test_dangling_backslash();
    test_accidental_pitch_boundaries();
    test_comment_newline_edge_cases();
    test_empty_and_whitespace_input();
    test_block_comment_newline_inside_interpolation();
    test_standalone_cr_inside_string_contexts();
    test_backslash_before_physical_newlines();

    printf("\n========================================\n");
    printf("TEST SUMMARY\n");
    printf("Checks run: %d\n", checks_run);
    printf("Checks failed: %d\n", checks_failed);
    printf("Checks passed: %d\n", checks_run - checks_failed);
    printf("========================================\n");

    return checks_failed == 0 ? 0 : 1;
}
