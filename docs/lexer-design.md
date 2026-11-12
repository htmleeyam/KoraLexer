
# Kora Lexer Design

## 1. Overview

The Kora Lexical Analyzer is a hand-written lexer implemented in C. Its purpose is to read Kora source code and convert it into a sequence of tokens that can be processed by later stages of a compiler or interpreter.

The lexer scans the source code character by character, identifies valid lexical patterns, and produces tokens containing the token type, original lexeme, and source location.

The current implementation uses helper functions to separate different scanning responsibilities.

## 2. Project Structure

The project is organized into source files, documentation, example programs, and automated tests.

| File or Directory | Description |
|---|---|
| `lexer.h` | Defines token types, data structures, lexer state, error codes, and function declarations. |
| `lexer.c` | Implements character scanning, token recognition, context management, and lexical error handling. |
| `main.c` | Loads a Kora source file, initializes the lexer, displays generated tokens, and reports lexical errors. |
| `examples/` | Contains Kora source files used to demonstrate token recognition and lexical error detection. |
| `tests/` | Contains the automated lexer regression test suite. |
| `docs/` | Contains the lexical specification, implementation audit, and design documentation. |

The automated test suite is located in `tests/lexer_tests.c`.

## 3. Lexer Architecture

The lexer follows a hand-written, procedural scanning approach.

It reads the source code from left to right and uses specialized functions to recognize different lexical elements.

The `Lexer` structure maintains the scanning state:

- `source` — a pointer to the original source code.
- `position` — the current character index in the source.
- `line` — the current source line.
- `column` — the current character position within the line.
- `inside_block_comment` — indicates whether the scanner is currently processing a block comment.
- `block_comment_start_line` — records the opening line of a block comment.
- `block_comment_start_column` — records the opening column of a block comment.
- `block_comment_start_position` — records the source position of the block comment's opening delimiter.
- `context_stack` — stores active string and interpolation contexts.
- `context_depth` — records the current number of active lexer contexts.

The general scanning process is:

1. Initialize the lexer with the source code and starting position.
2. Check the active lexer context, processing string content when appropriate.
3. Skip ordinary spaces and horizontal tabs.
4. Recognize and process comments while preserving physical newline behavior.
5. Handle end-of-source and physical newline conditions.
6. Recognize string boundaries and interpolation expressions.
7. Identify pitch literals, identifiers, keywords, and numeric literals.
8. Recognize operators and delimiters using the applicable matching rules.
9. Generate tokens containing their type, source lexeme, length, and starting location.
10. Continue scanning until the end of the source is reached.

The lexer uses a context stack to maintain nested string and interpolation states. Specialized scanning functions handle individual lexical elements, while `get_next_token()` coordinates the overall scanning process.

## 4. Token Representation

Tokens are represented using the `Token` structure declared in `lexer.h`.

The current implementation uses the following fields:

| Field | Description |
|---|---|
| `type` | Identifies the category of the token using the `TokenType` enumeration. |
| `lexeme` | A pointer to the original source characters that form the token. |
| `length` | The number of characters contained in the token's lexeme. |
| `line` | Records the source line where the token begins. |
| `column` | Records the column where the token begins. |
| `error_code` | Identifies the lexical error classification when the token is an error token. |

The lexer uses source-slice token representation. Instead of copying each lexeme into a fixed-size character array, a token stores a pointer into the original source and its corresponding length.

This allows the lexer to preserve complete lexemes, including identifiers and string contents longer than a fixed-size buffer.

The original source memory must remain available while tokens referencing it are being processed.

The `TokenType` enumeration defines the categories recognized by the lexer, including:

- Keywords, types, and interval units.
- Identifiers and literals.
- Arithmetic, assignment, comparison, and logical operators.
- Pipeline and arrow operators.
- Brackets, delimiters, and punctuation.
- Newlines, end-of-file markers, and lexical errors.
- String and interpolation tokens.

The current implementation preserves literal lexemes but does not convert or store separate literal values.

## 5. Main Scanning Functions

The implementation is divided into helper functions to make token recognition easier to understand and maintain.

### 5.1 `get_next_token()`

This is the main scanning function.

It determines what kind of token begins at the current source position and calls the appropriate helper function.

It also handles special lexer states, such as being inside a string or interpolation expression.

### 5.2 `make_token()`

Creates a token from the current source position.

It records the token type, a pointer to the original source lexeme, the lexeme length, and the starting line and column.

Instead of copying the lexeme into a separate character array, it preserves a source slice using a pointer and length.

It also advances the lexer's position and column according to the consumed token length.

This approach avoids fixed-size lexeme buffers and preserves the complete original source text.

### 5.3 `token_type_name()`

Converts a `TokenType` value into a readable string.

This is primarily used when displaying the generated tokens in the terminal.

### 5.4 `classify_identifier()`

Checks whether a scanned word belongs to a reserved-word category.

If the word does not match a reserved entry, it is classified as an identifier.

### 5.5 `read_identifier()`

Scans a sequence of valid identifier characters and classifies the resulting word.

### 5.6 `read_number()`

Recognizes integer and floating-point literals.

It also checks for certain malformed numeric sequences and returns an error token when an invalid form is encountered.

### 5.7 `is_pitch_literal()`

Checks whether the current source position matches the expected structure of a Kora pitch literal.

### 5.8 `read_operator_or_delimiter()`

Recognizes operators and punctuation.

Two-character operators are checked before single-character operators to support longest-match recognition.

### 5.9 `read_newline()`

Recognizes a newline and updates the lexer's line and column tracking.

CRLF sequences are treated as one logical newline.

### 5.10 `read_comment()`

Handles Kora's single-line and block comment forms.

Single-line comments begin with `~~`, while block comments begin with `~*` and end with `*~`.

### 5.11 `read_string_content()`

Scans the contents of a string and recognizes string boundaries and interpolation boundaries.

It also validates the supported escape sequences.

## 6. String Interpolation Handling

The lexer uses a bounded context stack to manage string literals and interpolation expressions.

The relevant data structures are:

| Structure or Variable | Purpose |
|---|---|
| `LexerContextType` | Identifies whether a context represents a string or interpolation expression. |
| `LexerContext` | Stores the context type and its brace depth. |
| `context_stack` | Stores the active nested string and interpolation contexts. |
| `context_depth` | Tracks the number of active contexts. |
| `MAX_LEXER_CONTEXTS` | Defines the maximum supported context depth of 64. |

The lexer uses three internal helper functions for context management:

- `push_context()` adds a new string or interpolation context.
- `pop_context()` removes the most recently active context.
- `current_context()` retrieves the currently active context.

When a string begins, the lexer pushes a string context onto the stack.

When an unescaped opening brace (`{`) is encountered inside string content, the lexer emits `INTERPOLATION_START` and pushes an interpolation context.

While processing an interpolation expression, the lexer uses brace-depth tracking to distinguish nested braces from the matching closing brace of the interpolation.

When the matching closing brace is found, the lexer emits `INTERPOLATION_END` and removes the interpolation context.

When a nested string appears inside an interpolation expression, another string context is pushed onto the same stack. This allows the lexer to preserve the correct parent context when nested strings and interpolations are entered and exited.

The lexer supports a maximum of 64 active contexts. If an attempt is made to exceed this limit, the lexer reports a `CONTEXT_LIMIT` error instead of writing beyond the allocated context stack.

The lexer uses the following token types for interpolated strings:

- `STRING_START`
- `STRING_TEXT`
- `INTERPOLATION_START`
- Normal expression tokens
- `INTERPOLATION_END`
- `STRING_END`

For example:

```kora
"Hello {name}!"
```

The lexer recognizes the string boundaries and text separately, while the expression inside the braces is processed using the regular token-recognition logic.

The lexer identifies the boundaries and tokens of the expression. Determining whether the expression is grammatically valid belongs to the parser.

## 7. Error Handling

The lexer uses `TOKEN_ERROR` to represent invalid lexical patterns.

Each error token contains an associated `LexicalErrorCode` that identifies the type of lexical error.

The current implementation supports the following error classifications:

| Error Code | Description |
|---|---|
| `LEX_ERROR_UNEXPECTED_CHARACTER` | An unsupported character is encountered. |
| `LEX_ERROR_INVALID_NUMBER` | A malformed numeric literal is encountered. |
| `LEX_ERROR_INVALID_PITCH` | A malformed pitch-like sequence is encountered. |
| `LEX_ERROR_INVALID_ESCAPE` | An unsupported string escape sequence is encountered. |
| `LEX_ERROR_UNTERMINATED_STRING` | A string reaches a physical line break or end of source without a closing quotation mark. |
| `LEX_ERROR_UNTERMINATED_BLOCK_COMMENT` | A block comment reaches the end of source without its closing marker. |
| `LEX_ERROR_INVALID_NEWLINE` | A standalone carriage return is encountered. |
| `LEX_ERROR_UNTERMINATED_INTERPOLATION` | An interpolation expression is terminated by a physical line break or reaches the end of source without its matching closing brace. |
| `LEX_ERROR_CONTEXT_LIMIT` | The maximum supported nested lexer context depth is exceeded. |

The diagnostic functions `lexical_error_message()` and `lexical_error_suggestion()` provide readable descriptions and suggested corrections for these error categories.

The `main.c` program displays the error description, associated lexeme, source location, and suggested correction.

The lexer attempts to continue scanning after recoverable lexical errors. Recovery behavior depends on the type of error and the current lexer context.

The suggestions are informational and do not automatically modify the source code.

## 8. Token Output

The `main.c` file serves as the command-line demonstration program for the Kora Lexical Analyzer.

The program accepts one `.kora` source file as a command-line argument, loads its contents into memory, and initializes the lexer using the loaded source.

It repeatedly calls `get_next_token()` until the lexer reaches `TOKEN_EOF`.

For each generated token, the program displays:

- The original lexeme.
- The corresponding token type.
- Additional error information when the token is a `TOKEN_ERROR`.

The program also displays a summary containing the total number of emitted tokens and the number of lexical errors encountered.

The source buffer remains allocated while the lexer processes the input because tokens reference lexemes from the original source. The buffer is released after lexical analysis is complete.

This output allows users to inspect how the lexer classifies different parts of a Kora source file and reports invalid lexical elements.


## 9. Implementation Notes

This document describes the current organization and architecture of the Kora Lexical Analyzer.

The implementation is a hand-coded, procedural scanner that uses specialized functions for token recognition, source tracking, comment handling, and string interpolation.

The automated regression test suite is located in `tests/lexer_tests.c`. It currently contains 26 test groups covering token recognition, lexical boundaries, nested contexts, error handling, newline behavior, and recovery.

In the latest reported test execution, all 584 individual checks passed with zero failures.

The implementation audit records the current verification results, known limitations, and differences between the lexer and its working lexical specification.

This design document should be updated whenever significant changes are made to the lexer architecture, data structures, or scanning responsibilities.
