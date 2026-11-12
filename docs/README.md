# Kora Lexer Documentation

This folder contains the documentation for the Kora Lexical Analyzer. These documents define the intended lexical rules of the Kora programming language, describe the lexer architecture, and evaluate how those rules are implemented.

## Documents

### 1. Kora Lexical Specification

**File:** `Kora-Lexical-Spec-v1.0.md`

This document defines the lexical rules and token structures that the Kora lexer is expected to recognize.

It covers:

- Character sets and whitespace
- Keywords and reserved words
- Identifiers and literals
- Pitch and interval notation
- Operators and delimiters
- Strings and escape sequences
- String interpolation
- Single-line and multi-line comments
- Lexical errors and expected scanner behavior

**Purpose:** Defines what the lexer should recognize and how Kora source code should be tokenized.

### 2. Kora Lexical Audit

**File:** `Kora-Lexical-Audit.md`

This document examines the current lexer implementation and compares its behavior against the lexical specification.

It records:

- Implemented lexical features
- Verified test results
- Differences between the specification and implementation
- Error detection and reporting behavior
- Known limitations and areas for improvement

**Purpose:** Evaluates how closely the implementation follows the lexical specification.

### 3. Lexer Design

**File:** `lexer-design.md`

This document describes the architecture and organization of the Kora Lexical Analyzer.

It covers:

- Overall lexer architecture
- Source tracking and token representation
- Main scanning functions
- String and interpolation context management
- Lexical error handling
- Token output and implementation details

**Purpose:** Explains how the lexer is structured and how its scanning components work together.

## Relationship to the Source Code

| File | Responsibility |
|---|---|
| `lexer.h` | Defines token types, token structures, error codes, and function declarations. |
| `lexer.c` | Implements character scanning, token recognition, and lexical error handling. |
| `main.c` | Loads Kora source files, runs the lexer, and displays tokens, lexical errors, and the analysis summary. |
| `tests/lexer_tests.c` | Contains automated regression tests for token recognition, lexical boundaries, and error handling. |

## Lexical Error Handling

The lexer includes a lexical error-reporting mechanism.

When an invalid lexical element is encountered, the implementation can provide:

- An error description
- The associated invalid lexeme
- The source line and column
- A suggested correction

The lexer also attempts to continue scanning after recoverable errors so that multiple problems can be reported during one analysis.

Suggestions are informational. Automatic source-code correction is not part of the lexer.

## How to Use These Documents

1. Read the **Kora Lexical Specification** to understand the language's lexical rules.
2. Examine the **Kora Lexical Audit** to review implemented features and verified test results.
3. Read **Lexer Design** to understand the architecture and implementation approach.
4. Review the source code to understand how the lexer processes input.
5. Run the example files and automated tests to verify token recognition and lexical error handling.
6. Update the specification, audit, and design documentation whenever an agreed language rule or implementation behavior changes.

## Documentation Status

These documents evolve alongside the Kora Lexer implementation.

The lexical specification serves as the reference for expected behavior, the audit records the implementation's current status, and the design document explains the architecture.

Changes to the language rules, implementation behavior, or lexer architecture should be reflected in the appropriate document and supported by testing.
