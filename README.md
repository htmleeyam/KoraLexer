# Kora Lexical Analyzer 

A hand-coded lexical analyzer (lexer) implementation for **Kora**, a domain-specific programming language designed primarily for musicians and composers to create music through code.

The lexer reads Kora source code character by character and converts meaningful sequences called **lexemes** into tokens that can be processed by the next stage of the compiler.

## Project Overview

The lexical analyzer identifies and classifies elements of Kora source code, including keywords, identifiers, literals, operators, delimiters, strings, and comments.

It also detects invalid lexical elements and provides diagnostic information to help developers identify and correct errors.

## Current Features

### Token Recognition
- Keyword and reserved-word recognition
- Identifier recognition
- Integer and floating-point literal recognition
- Boolean literal recognition
- Pitch literal recognition
- Interval-unit recognition
- Operator and delimiter recognition
- Newline recognition

### Strings and Comments
- String literal recognition
- String interpolation
- Nested string and interpolation contexts
- String escape sequence recognition
- Single-line comments (`~~`)
- Multi-line comments (`~* ... *~`)

### Lexical Error Handling and Reporting
- Unexpected character detection
- Malformed numeric literal detection
- Malformed pitch literal detection
- Invalid string escape detection
- Unterminated string detection
- Unterminated interpolation detection
- Unterminated block comment detection
- Error descriptions and source locations
- Rule-based correction suggestions
- Continued scanning after recoverable lexical errors

## Project Structure

```text
KoraLexer/
│
├── main.c
├── lexer.c
├── lexer.h
│
├── examples/
│   ├── test_features.kora
│   └── test_error-detection.kora
│
├── docs/
│   ├── README.md
│   ├── Kora-Lexical-Spec-v1.0.md
│   └── Kora-Lexical-Audit.md
│
├── README.md
└── .gitignore
```

## Requirements

- GCC Compiler
- Visual Studio Code (recommended)
- Git

## Compilation and Execution

Compile the program:

```bash
gcc main.c lexer.c -o kora.exe
```

Run a Kora source file:

```powershell
.\kora.exe examples\test_features.kora
```

Run the lexical error test:

```powershell
.\kora.exe examples\test_error-detection.kora
```

## Sample Input

```kora
fn main() {
    int score = 100
    float average = 95.5

    "Hello, Kora!"
}
```

## Sample Token Output

| Lexeme | Token Type |
|---|---|
| `fn` | KEYWORD |
| `main` | IDENTIFIER |
| `(` | LEFT_PAREN |
| `)` | RIGHT_PAREN |
| `{` | LEFT_BRACE |
| `int` | TYPE |
| `score` | IDENTIFIER |
| `=` | ASSIGN |
| `100` | INTEGER |
| `float` | TYPE |
| `average` | IDENTIFIER |
| `95.5` | FLOAT |

## Lexical Error Reporting

When the lexer encounters an invalid lexical element, it reports the error instead of silently ignoring it.

Example input:

```kora
int score = 100 @
```

Example diagnostic:

```text
LEXICAL ERROR
Description : Unexpected character.
Lexeme      : '@'
Location    : Line 1, Column 17
Suggestion  : Remove the character or replace it with a valid Kora token.
```

Each diagnostic can contain:

- **Description** – explains the detected problem.
- **Lexeme** – identifies the invalid source sequence.
- **Location** – indicates the line and column.
- **Suggestion** – provides a possible correction.

Suggestions are informational and do not automatically modify the source code.

## Development Status

**Status: In Development**

The lexer currently supports core token recognition, string and comment handling, string interpolation, and lexical error reporting.

Further development focuses on testing, refinement, documentation, and maintaining consistency between the implementation and the Kora lexical specification.

## Documentation

The `docs/` directory contains:

- **Kora Lexical Specification** – defines the expected lexical rules and token structures of Kora.
- **Kora Lexical Audit** – evaluates the current implementation against the lexical specification.

## Language Project

Kora is a domain-specific programming language focused on music composition and scripting. This repository contains the lexical analysis component of the larger language implementation.
