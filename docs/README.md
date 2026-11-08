
# Kora Lexer Documentation

This folder contains the documentation for the Kora Lexical Analyzer. These documents describe the intended lexical rules of the Kora programming language and evaluate how those rules are implemented in the lexer.

## Documents

### 1. Kora Lexical Specification

**File:** `Kora-Lexical-Spec-v1.0.md`

This document defines the lexical rules and token structures that the Kora lexer is expected to recognize.

It serves as the reference for:
- Keywords and reserved words
- Identifiers and literals
- Operators and delimiters
- Comments and whitespace
- String and escape sequence rules
- Other lexical conventions of Kora

**Purpose:** Defines what the lexer should recognize.

### 2. Kora Lexical Audit

**File:** `Kora-Lexical-Audit.md`

This document examines the current lexer implementation and compares its behavior against the lexical specification.

It helps identify:
- Implemented lexical features
- Features that need further testing
- Differences between the specification and implementation
- Potential issues and improvements

**Purpose:** Checks how closely the implementation follows the specification.

## Relationship to the Source Code

The documentation describes the expected behavior and implementation status of the lexer.

The main source files are:

| File | Responsibility |
|---|---|
| `lexer.h` | Defines token types and lexer data structures. |
| `lexer.c` | Implements the lexical analysis and tokenization logic. |
| `main.c` | Runs the lexer and displays the resulting tokens. |

## How to Use These Documents

1. Read the **Kora Lexical Specification** to understand the language's lexical rules.
2. Examine the **Kora Lexical Audit** to see which rules are implemented and which need attention.
3. Review the source code to understand how the lexer processes the input.
4. Update the audit when implementation behavior is verified through testing.

## Documentation Status

These documents are intended to evolve alongside the Kora Lexer implementation. Any changes to the language specification or lexer behavior should be reflected in the relevant documentation.