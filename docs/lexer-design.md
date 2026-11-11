
# Kora Lexer Design

## 1. Overview

The Kora Lexical Analyzer is a hand-written lexer implemented in C. Its purpose is to read Kora source code and convert it into a sequence of tokens that can be processed by later stages of a compiler or interpreter.

The lexer scans the source code character by character, identifies valid lexical patterns, and produces tokens containing the token type, original lexeme, and source location.

The current implementation uses helper functions to separate different scanning responsibilities.

## 2. Project Structure

The current project uses a simple file structure:

| File | Description |
|---|---|
| `lexer.h` | Contains token definitions, data structures, lexer state variables, and function declarations. |
| `lexer.c` | Contains the implementation of token recognition, scanning, and lexical analysis. |
| `main.c` | Initializes the lexer, supplies sample Kora source code, and displays the generated tokens. |

The project also contains a `docs/` directory for the lexical specification, implementation audit, and design documentation.

## 3. Lexer Architecture

The lexer follows a hand-written scanning approach.

It maintains a reference to the source code and tracks its current position using:

- `position` — the current character index in the source.
- `line` — the current source line.
- `column` — the current character position within the line.

The lexer also maintains additional state variables for handling block comments and string interpolation.

The general scanning process is:

1. Initialize the lexer with the source code.
2. Read the character at the current source position.
3. Skip ordinary spaces and tabs.
4. Check for comments and handle them accordingly.
5. Recognize newlines and special string boundaries.
6. Identify literals, identifiers, keywords, operators, and delimiters.
7. Generate a token containing its type, lexeme, and source location.
8. Repeat until the end of the source is reached.

## 4. Token Representation

Tokens are represented using the `Token` structure declared in `lexer.h`.

Each token contains:

| Field | Description |
|---|---|
| `type` | Identifies the category of the token. |
| `lexeme` | Stores the original text recognized from the source code. |
| `line` | Records the source line where the token begins. |
| `column` | Records the column where the token begins. |

The `TokenType` enumeration defines the token categories recognized by the lexer, including:

- Keywords and data types
- Identifiers and literals
- Arithmetic, assignment, comparison, and logical operators
- Pipeline and arrow operators
- Brackets, delimiters, and punctuation
- Newlines, end-of-file markers, and lexical errors
- String and interpolation tokens

## 5. Main Scanning Functions

The implementation is divided into helper functions to make token recognition easier to understand and maintain.

### 5.1 `get_next_token()`

This is the main scanning function.

It determines what kind of token begins at the current source position and calls the appropriate helper function.

It also handles special lexer states, such as being inside a string or interpolation expression.

### 5.2 `make_token()`

Creates a token using the current source position.

It assigns the token type, records the starting line and column, copies the recognized lexeme, and advances the lexer's position.

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

The lexer uses additional state variables to process strings containing interpolation expressions.

The relevant state variables are:

| Variable | Purpose |
|---|---|
| `inside_string` | Indicates whether the lexer is currently processing a string. |
| `inside_interpolation` | Indicates whether the lexer is processing an expression inside a string. |
| `interpolation_brace_depth` | Tracks nested braces within an interpolation expression. |

The lexer uses the following token types for interpolated strings:

- `STRING_START`
- `STRING_TEXT`
- `INTERPOLATION_START`
- `INTERPOLATION_END`
- `STRING_END`

For example:

```kora
"Hello {name}!"
```

The lexer recognizes the string boundaries and text separately, while the expression inside the braces is processed using the regular token-recognition logic.

The lexer identifies the boundaries and tokens of the expression. Determining whether the expression is grammatically valid belongs to the parser.

## 7. Error Handling

The lexer uses `TOKEN_ERROR` to represent certain invalid lexical patterns.

Examples include:

- Invalid numeric literals
- Invalid pitch-like sequences
- Unsupported characters
- Invalid string escape sequences
- Unterminated strings or interpolation expressions
- Unterminated block comments

The lexer should report errors without preventing it from processing subsequent source code whenever recovery is possible.

## 8. Token Output

The `main.c` file serves as the current demonstration program.

It initializes the lexer with sample Kora source code, repeatedly calls `get_next_token()`, and displays each token's lexeme and token type.

The output makes it possible to inspect how the lexer classifies different parts of the source code.

## 9. Implementation Notes

This document describes the current organization of the lexer, not a guarantee that every rule in the Kora Lexical Specification has been fully implemented.

Some behaviors still require additional testing and verification against the specification.

The implementation audit records known differences, unresolved edge cases, and areas that require further work.

As the lexer develops, this document should be updated when its architecture or scanning responsibilities change.