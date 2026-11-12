
# Kora Lexical Audit

## 1. Purpose

This document evaluates the current implementation of the Kora Lexical Analyzer against the rules defined in `Kora-Lexical-Spec-v1.0.md`.

The audit records implemented features, verified test results, differences between the specification and implementation, and areas that may require further refinement.

The audit is based on the current source code and the test outputs available at the time of review.

## 2. Implementation Overview

The Kora Lexical Analyzer uses a **hand-coded, procedural scanning approach**.

The main scanning function reads source code character by character and delegates token recognition to specialized functions for different lexical elements.

The implementation includes:

- Token recognition and classification.
- Identifier and reserved-word recognition.
- Numeric and pitch literal recognition.
- Operator and delimiter recognition.
- String and interpolation processing.
- Single-line and multi-line comment handling.
- Source line and column tracking.
- Lexical error detection and reporting.

The lexer also maintains a context stack to support nested strings and interpolation expressions.

## 3. Implemented Features

| Feature | Implementation status | Description |
|---|---|---|
| Keyword recognition | Implemented | Recognizes the 38 reserved lexemes and classifies them into the appropriate token categories. |
| Identifier recognition | Implemented | Recognizes identifiers using the defined naming rules and preserves their complete lexemes. |
| Integer literals | Implemented | Recognizes integer literal lexemes without converting them into numeric values. |
| Floating-point literals | Implemented | Recognizes decimal numeric lexemes according to the specified format. |
| Boolean literals | Implemented | Classifies `true` and `false` using the `BOOLEAN` token type. |
| Pitch literals | Implemented | Recognizes valid pitch-shaped sequences and detects specified malformed pitch-like sequences. |
| Interval units | Implemented | Recognizes `st` and `oct` as interval-unit tokens. |
| Operators | Implemented | Recognizes supported arithmetic, assignment, comparison, logical, and other operators using longest-match recognition. |
| Delimiters | Implemented | Recognizes parentheses, braces, brackets, commas, colons, dots, and semicolons. |
| Newline handling | Implemented | Recognizes LF and CRLF as logical newlines and reports standalone CR as a lexical error. |
| Single-line comments | Implemented | Recognizes and skips `~~` comments while preserving the following physical newline. |
| Multi-line comments | Implemented | Recognizes and skips `~* ... *~` comments, preserves physical newlines, and detects unterminated block comments. |
| String literals | Implemented | Recognizes string boundaries and string content. |
| String escapes | Implemented | Recognizes all seven supported escape sequences and reports unsupported escape sequences. |
| String interpolation | Implemented | Recognizes interpolation boundaries and normal expression tokens inside interpolation. |
| Nested contexts | Implemented | Uses a bounded context stack to handle nested strings and interpolation expressions. |
| Lexical error reporting | Implemented | Reports error descriptions, lexemes, source locations, and correction suggestions. |
| Error recovery | Implemented with limitations | Continues scanning after tested recoverable lexical errors. Recovery behavior depends on the type and context of the error. |

## 4. Lexical Error Handling and Reporting

The lexer includes a diagnostic mechanism for identifying invalid lexical elements.

When an error is encountered, the implementation reports:

- **Description:** A readable explanation of the error.
- **Lexeme:** The invalid character or source sequence associated with the error.
- **Location:** The line and column where the lexer encountered the problem.
- **Suggestion:** A possible correction based on the error category.

### 4.1 Supported Error Categories

| Error category | C Error Code | Example | Reported behavior |
|---|---|---|---|
| Unexpected character | `LEX_ERROR_UNEXPECTED_CHARACTER` | `@` | Identifies an unsupported character. |
| Malformed numeric literal | `LEX_ERROR_INVALID_NUMBER` | `12.3.4` | Reports an invalid numeric format. |
| Malformed pitch literal | `LEX_ERROR_INVALID_PITCH` | `C#` | Reports an incomplete pitch notation. |
| Invalid string escape | `LEX_ERROR_INVALID_ESCAPE` | `\q` | Identifies an unsupported escape sequence. |
| Unterminated string | `LEX_ERROR_UNTERMINATED_STRING` | Missing closing `"` | Reports a string that reaches a physical line break or end of source without being closed. |
| Unterminated interpolation | `LEX_ERROR_UNTERMINATED_INTERPOLATION` | Missing `}` | Reports an interpolation expression that reaches a physical line break or end of source without being closed. |
| Unterminated block comment | `LEX_ERROR_UNTERMINATED_BLOCK_COMMENT` | Missing `*~` | Reports a block comment that reaches the end of the source without its closing marker. |
| Invalid newline | `LEX_ERROR_INVALID_NEWLINE` | Standalone `\r` | Reports a carriage return that is not followed by LF. |
| Context-stack overflow | `LEX_ERROR_CONTEXT_LIMIT` | Excessive nested string/interpolation contexts | Reports that the maximum supported context depth has been exceeded. |

### 4.2 Example Diagnostic Output

For the input:

```kora
int score = 100 @
```

The lexer produces:

```text
LEXICAL ERROR
Description : Unexpected character.
Lexeme      : '@'
Location    : Line 1, Column 17
Suggestion  : Remove the character or replace it with a valid Kora token.
```

This allows the programmer to identify the issue without manually inspecting the entire source file.

### 4.3 Error Recovery

The lexer attempts to continue scanning after recoverable lexical errors.

This allows multiple invalid elements to be identified during one execution instead of stopping at the first error.

However, recovery depends on the type of error. An unterminated construct at the end of the file, such as a block comment, naturally prevents recognition of tokens beyond that construct.

The suggestions are informational and do not automatically modify the source code.

## 5. Verification and Test Results

### 5.1 Lexical Error Detection Test

**Test file:** `examples/test_error-detection.kora`

The test file contains deliberately invalid lexical elements to verify the diagnostic mechanism.

| Test | Input | Expected result | Observed result |
|---|---|---|---|
| 1 | `@` | Unexpected character | Passed |
| 2 | `12.3.4` | Malformed numeric literal | Passed |
| 3 | `C#` | Malformed pitch literal | Passed |
| 4 | `\q` | Invalid escape sequence | Passed |
| 5 | Unclosed string | Unterminated string | Passed |
| 6 | Unclosed interpolation | Unterminated interpolation | Passed |
| 7 | Unclosed block comment | Unterminated block comment | Passed |

**Observed summary:**

```text
Total tokens: 58
Lexical errors: 7
Lexical analysis completed with errors.
```

All seven deliberately introduced errors were detected and accompanied by descriptions and correction suggestions.

The output also demonstrates that the lexer continued recognizing subsequent tokens after recoverable errors.

### 5.2 Automated Regression Test Suite

**Test file:** `tests/lexer_tests.c`

The project includes an automated regression test suite covering token recognition, lexical boundaries, error detection, source locations, and recovery behavior.

The suite currently contains 26 test groups:

| Test | Coverage |
|---|---|
| 1 | Basic Token Recognition |
| 2 | Long Identifier Preservation |
| 3 | CRLF Newline Handling |
| 4 | Keyword Boundary Recognition |
| 5 | Pitch Literal Boundaries |
| 6 | Numeric Literal Validation |
| 7 | Operators and Delimiters |
| 8 | Adjacent Operators and Longest Match |
| 9 | String Literals and Escape Sequences |
| 10 | Basic String Interpolation |
| 11 | Nested Braces and Nested Strings |
| 12 | Unterminated Strings and Recovery |
| 13 | Unterminated Interpolation and Recovery |
| 14 | Single-Line and Block Comments |
| 15 | Lexer Context Stack Limit |
| 16 | Standalone Carriage Return |
| 17 | Complete Reserved-Word Inventory |
| 18 | Unexpected Characters and Error Recovery |
| 19 | Escaped Opening Brace |
| 20 | Dangling Backslash at End of String |
| 21 | Accidental Pitch Boundaries |
| 22 | Newline Edge Cases Inside Block Comments |
| 23 | Empty and Whitespace-Only Input |
| 24 | Block Comment Newlines Inside Interpolation |
| 25 | Standalone CR Inside Strings and Interpolation |
| 26 | Backslash Before Physical Newlines |

**Latest reported test result:**

```text
========================================
TEST SUMMARY
Checks run: 584
Checks failed: 0
Checks passed: 584
========================================
```

All 584 checks passed in the latest reported test execution.

The suite verifies token types, lexemes, source locations, lexical error classifications, context handling, and recovery behavior.

To compile and run the automated tests:

```powershell
gcc -Wall -Wextra -std=c11 tests/lexer_tests.c lexer.c -o tests/lexer_tests.exe

.\tests\lexer_tests.exe
```

The automated regression suite complements the standalone example files by testing specific lexical rules and boundary cases individually.

## 6. Differences and Areas for Improvement

Although the current lexer supports the major features described in the working specification, some areas still deserve attention.

| Area | Current observation | Remaining consideration |
|---|---|---|
| Error source locations | The lexer reports the location associated with the detected error. For unterminated block comments, it reports the opening delimiter. For physical newlines inside strings and interpolation, it reports the newline location. | The implementation could optionally retain opening quotation-mark or interpolation locations for additional diagnostic context. |
| Error recovery | Recovery behavior has been tested for unexpected characters, malformed literals, invalid escapes, unterminated strings, interpolation errors, and newline-related errors. | More complicated combinations of malformed nested constructs may require additional tests if new language rules are introduced. |
| Standalone carriage return | Tested outside strings, inside strings, inside interpolation, and inside block comments. It produces `INVALID_NEWLINE` without incorrectly incrementing the line number. | No known outstanding issue from the current regression tests. |
| Context-stack limit | The implementation supports up to 64 nested lexer contexts and detects an attempt to exceed that limit. | The fixed maximum depth remains an implementation constraint. |
| Pitch boundaries | Valid pitch forms, accidental notation, incomplete pitch forms, and identifier boundaries are covered by the regression suite. | Additional cases may be added if the pitch grammar changes. |
| String newline and escape handling | All seven supported escape sequences are recognized. Backslash followed by LF, CRLF, or standalone CR is covered by Test 26. | Recovery for newly introduced string syntax or escape rules should be tested before implementation. |
| Specification consistency | The lexical specification has been updated to reflect the current token structure, token names, scanning order, and error classifications. | The working specification may still require final team sign-off. |

## 7. Conclusion

The Kora Lexical Analyzer implements the major token-recognition features defined in its working lexical specification.

Its hand-coded, procedural scanning approach separates token recognition into specialized functions. The implementation supports identifiers, reserved words, numeric and pitch literals, operators, delimiters, strings, interpolation, comments, and physical newline handling.

The lexer also provides categorized lexical errors, source locations, correction suggestions, and recovery behavior for tested invalid inputs.

Nested strings and interpolation expressions are managed through a bounded context stack, while source-slice token lexemes preserve the original source characters without relying on fixed-size lexeme buffers.

The automated regression suite currently contains 26 test groups and 584 individual checks. In the latest reported execution, all 584 checks passed with zero failures.

Although the core lexical features and tested edge cases are implemented, the lexer remains part of the larger Kora language development process. The working lexical specification may still require final team sign-off, and additional testing may be necessary as the language evolves.

Successful lexical analysis does not guarantee that a complete Kora program is syntactically or semantically valid. Those responsibilities belong to the parser and subsequent compiler stages.