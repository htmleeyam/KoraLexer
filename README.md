# Kora Lexical Analyzer

A hand-coded lexical analyzer (lexer) implementation for **Kora**, a domain-specific programming language designed primarily for musicians and composers to create music through code.

The lexer reads Kora source code character by character and converts meaningful sequences called **lexemes** into tokens that can be processed by the next stage of the compiler.

## Project Overview

The Kora Lexical Analyzer identifies and classifies elements of Kora source code, including keywords, identifiers, literals, operators, delimiters, strings, and comments.

It also detects invalid lexical elements and provides diagnostic information to help developers identify and correct errors.

## Current Features

### Token Recognition
- Recognition of 38 reserved lexemes
- Identifier recognition
- Integer and floating-point literal recognition
- Boolean literal recognition
- Pitch literal recognition
- Interval-unit recognition
- Operator and delimiter recognition
- Newline recognition

### Strings and Comments
- String literal recognition
- Seven supported string escape sequences
- String interpolation
- Nested string and interpolation contexts
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
- Invalid standalone carriage return detection
- Maximum nested context-depth detection
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
│   ├── 01_keywords_types.kora
│   ├── 02_literals.kora
│   ├── 03_operators.kora
│   ├── 04_delimiters.kora
│   ├── 05_comments.kora
│   ├── 06_strings.kora
│   ├── 07_interpolation.kora
│   ├── 08_combined_program.kora
│   ├── 09_invalid_inputs.kora
│   ├── 10_newlines.kora
│   ├── kora-example.kora
│   ├── test_error-detection.kora
│   └── test_features.kora
│
├── tests/
│   └── lexer_tests.c
│
├── docs/
│   ├── Kora-Lexical-Audit.md
│   ├── Kora-Lexical-Spec-v1.0.md
│   ├── lexer-design.md
│   └── README.md
│
├── .vscode/
│   └── launch.json
│
├── README.md
└── .gitignore
```

## Requirements

- GCC Compiler
- C compiler supporting the C11 standard
- Visual Studio Code (recommended)
- Git

## Compilation and Execution

Compile the program using GCC:

```powershell
gcc -Wall -Wextra -std=c11 main.c lexer.c -o kora.exe
```

Run a Kora source file:

```powershell
.\kora.exe examples\test_features.kora
```

Run the lexical error demonstration:

```powershell
.\kora.exe examples\test_error-detection.kora
```

The program accepts one `.kora` source file at a time.

## Running Automated Tests

The project includes an automated regression test suite covering token recognition, lexical boundaries, string handling, interpolation, comments, newline behavior, and lexical error recovery.

Compile the test suite:

```powershell
gcc -Wall -Wextra -std=c11 tests/lexer_tests.c lexer.c -o tests/lexer_tests.exe
```

Run the tests:

```powershell
.\tests\lexer_tests.exe
```

The latest reported test execution produced:

```text
========================================
TEST SUMMARY
Checks run: 584
Checks failed: 0
Checks passed: 584
========================================
```

The suite currently contains 26 test groups.

## Sample Input

```kora
let count = 42
```

## Sample Token Output

| Lexeme | Token Type |
|---|---|
| `let` | KEYWORD |
| `count` | IDENTIFIER |
| `=` | ASSIGN |
| `42` | INTEGER |

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

## Documentation

The `docs/` directory contains:

- **Kora Lexical Specification** – defines the expected lexical rules, reserved words, token structures, and language conventions.
- **Kora Lexical Audit** – evaluates the current implementation against the lexical specification.
- **Lexer Design** – explains the architecture and implementation approach of the lexical analyzer.
- **Documentation README** – provides an overview of the project's documentation.

## Development Status

**Status: Core lexical features implemented; final validation and documentation are ongoing.**

The lexer supports the major lexical features defined in its working specification and includes an automated regression test suite.

Further development may involve additional edge-case testing, specification refinement, and alignment with future changes to the Kora language.

The lexical specification remains a working document and may require final team sign-off.

## Language Project

Kora is a domain-specific programming language focused on music composition and scripting. This repository contains the lexical analysis component of the larger language implementation.

Successful lexical analysis does not guarantee that a complete Kora program is syntactically or semantically valid. Those responsibilities belong to later compiler stages.
