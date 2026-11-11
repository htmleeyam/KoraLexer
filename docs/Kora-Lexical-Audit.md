
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
| Keyword recognition | Implemented | Recognizes reserved words and classifies them into token categories. |
| Identifier recognition | Implemented | Recognizes valid identifiers. |
| Integer literals | Implemented | Recognizes integer values. |
| Floating-point literals | Implemented | Recognizes decimal numeric values. |
| Boolean literals | Implemented | Recognizes Boolean values. |
| Pitch literals | Implemented | Recognizes valid pitch notation and detects certain malformed pitch sequences. |
| Interval units | Implemented | Recognizes `st` and `oct` as interval-unit tokens. |
| Operators | Implemented | Recognizes supported arithmetic, assignment, comparison, logical, and other operators. |
| Delimiters | Implemented | Recognizes parentheses, braces, brackets, commas, colons, dots, and semicolons. |
| Newline handling | Implemented | Recognizes LF and CRLF line endings and emits newline tokens. |
| Single-line comments | Implemented | Recognizes and skips `~~` comments. |
| Multi-line comments | Implemented | Recognizes and skips `~* ... *~` comments and detects unterminated block comments. |
| String literals | Implemented | Recognizes string boundaries and string content. |
| String escapes | Implemented | Recognizes supported escape sequences and reports unsupported ones. |
| String interpolation | Implemented | Recognizes interpolation boundaries and normal expression tokens inside interpolation. |
| Nested contexts | Implemented | Uses a context stack to handle nested strings and interpolation expressions. |
| Lexical error reporting | Implemented | Reports error descriptions, lexemes, locations, and suggestions. |
| Error recovery | Implemented with limitations | Continues scanning after recoverable lexical errors. |

## 4. Lexical Error Handling and Reporting

The lexer includes a diagnostic mechanism for identifying invalid lexical elements.

When an error is encountered, the implementation reports:

- **Description:** A readable explanation of the error.
- **Lexeme:** The invalid character or source sequence associated with the error.
- **Location:** The line and column where the lexer encountered the problem.
- **Suggestion:** A possible correction based on the error category.

### 4.1 Supported Error Categories

| Error category | Example | Reported behavior |
|---|---|---|
| Unexpected character | `@` | Identifies an unsupported character. |
| Malformed numeric literal | `12.3.4` | Reports an invalid numeric format. |
| Malformed pitch literal | `C#` | Reports an incomplete pitch notation. |
| Invalid string escape | `\q` | Identifies an unsupported escape sequence. |
| Unterminated string | Missing closing `"` | Reports a string that reaches a physical newline without being closed. |
| Unterminated interpolation | Missing `}` | Reports an interpolation expression that is not closed. |
| Unterminated block comment | Missing `*~` | Reports a multi-line comment that reaches the end of the source without its closing marker. |

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

### 5.2 Other Lexer Tests

Separate example files are also used to test:

- Keyword and type recognition.
- Numeric and Boolean literals.
- Operators and delimiters.
- Single-line and multi-line comments.
- String literals and escape sequences.
- String interpolation and nested contexts.
- Combined Kora source-code examples.
- Invalid input handling.
- Newline and line-ending behavior.

These tests complement the error-detection test by checking normal tokenization and boundary behavior.

## 6. Differences and Areas for Improvement

Although the current lexer supports the major features described in the working specification, some areas still deserve attention.

| Area | Observation | Recommended action |
|---|---|---|
| Error source locations | Unterminated string and interpolation errors currently identify the newline where the problem is detected. | Consider reporting the opening quotation mark or interpolation delimiter as an additional location. |
| Error recovery | Recovery is implemented for the tested errors, but behavior may vary for more complicated malformed input. | Test multiple errors on the same line and malformed nested constructs. |
| Standalone carriage return | The specification requires a lexical error for a standalone CR. | Verify this using a test file containing an actual standalone CR byte. |
| Context-stack limit | The implementation uses a fixed maximum context depth. | Test excessive nested strings and interpolation contexts. |
| Pitch boundaries | Certain malformed pitch forms are detected, but not every possible pitch-like sequence has been tested. | Expand pitch boundary and malformed-literal tests. |
| Specification consistency | Some specification decisions may differ from the actual implementation. | Continue comparing the scanner behavior with the latest approved lexical rules. |

## 7. Conclusion

The current Kora Lexical Analyzer implements the major token-recognition features required by its working lexical specification.

In addition to recognizing valid tokens, it provides lexical error detection and reporting, including descriptions, source locations, and rule-based correction suggestions.

The implementation also attempts to recover from errors and continue scanning, allowing multiple lexical issues to be reported in one execution.

Further work should focus on edge-case testing, refining error locations, and maintaining consistency between the implementation and the language specification.

The lexer is still under development, and its successful tokenization does not guarantee that a complete Kora program is syntactically or semantically valid.
