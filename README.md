# Kora Lexical Analyzer

A lexical analyzer (lexer) implementation for **Kora**, a domain-specific programming language designed primarily for musicians and composers to create music through code.

This project is responsible for reading Kora source code and converting it into a sequence of tokens that can be processed by the next stage of the compiler.

## Project Overview

The lexer scans the source code character by character and identifies meaningful sequences called **lexemes**. Each lexeme is classified into its corresponding token type.

For example:

```kora
let result = 10 + 3.14 * 2
```

The lexer identifies the keywords, identifiers, operators, integer literals, and floating-point literals in the source code.

## Current Features

- Keyword recognition
- Identifier recognition
- Operator recognition
- Delimiter recognition
- Integer literal recognition
- Floating-point literal recognition
- Token classification and display

## Project Structure

```text
KoraLexer/
│
├── main.c       # Main program and token display
├── lexer.c      # Lexical analysis implementation
├── lexer.h      # Token definitions and function declarations
├── test.kora    # Kora source code for testing
├── README.md    # Project documentation
└── .gitignore   # Files excluded from Git tracking
```

## Requirements

- GCC Compiler
- Visual Studio Code (recommended)
- Git (for version control)

## Compilation and Execution

Open the project folder in VS Code and launch the integrated terminal.

Compile the program:

```bash
gcc main.c lexer.c -o main.exe
```

Run the executable:

```bash
.\main.exe
```

## Sample Input

```kora
let result = 10 + 3.14 * 2
```

## Sample Token Output

| Lexeme | Token |
|---|---|
| let | KEYWORD |
| result | IDENTIFIER |
| = | ASSIGN |
| 10 | INTEGER |
| + | PLUS |
| 3.14 | FLOAT |
| * | MULTIPLY |
| 2 | INTEGER |

## Development Status

**Status: In Development**

The lexer is being developed incrementally. Additional language features and token types will be implemented as the Kora language specification is finalized.

Planned additions include:

- String literals and string interpolation
- Pitch literals
- Interval literals
- Boolean literals
- Comments
- Newline handling
- Source-code error reporting

## Language Project

Kora is a domain-specific language focused on music composition and scripting. This repository contains the lexical analysis component of the larger language implementation.
