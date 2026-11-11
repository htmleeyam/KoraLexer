# Kora Lexer Documentation

This folder contains the documentation for the Kora Lexical Analyzer. These documents define the intended lexical rules of the Kora programming language and evaluate how those rules are implemented in the lexer.

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

## Relationship to the Source Code

| File | Responsibility |
|---|---|
| `lexer.h` | Defines token types, token structures, error codes, and function declarations. |
| `lexer.c` | Implements character scanning, token recognition, and lexical error handling. |
| `main.c` | Runs the lexer and displays tokens, lexical errors, and the analysis summary. |

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
3. Review the source code to understand how the lexer processes input.
4. Run the example files to test token recognition and lexical error reporting.
5. Update the specification and audit whenever an agreed language rule or implementation behavior changes.

## Documentation Status

These documents evolve alongside the Kora Lexer implementation.

The lexical specification serves as the reference for expected behavior, while the audit records the implementation's current status. Changes to either the language rules or lexer behavior should be reflected in the appropriate document and supported by testing.
