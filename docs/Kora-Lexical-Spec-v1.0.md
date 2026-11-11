# Kora Lexical Specification v1.0

**Status:** Working specification for the Kora hand-coded lexical analyzer; updated to reflect the implementation decisions recorded so far. Final team sign-off may still be required.

**Purpose:** Define how the lexer converts Kora source characters into tokens.

**Implementation constraint:** Recognition must be implemented manually with a character scanner. Standard language data structures (for example, lists, dictionaries, and enums) are allowed; lexer generators, parser generators, regex-based tokenizers, and compiler frameworks are out of scope.

---

## 1. Source and revision authority

Use the established Kora specification in the first part of the documentation, including its first revision, as the baseline. Apply the additional material under **MORE REVISIONS**, followed by the consolidated **RESERVED WORDS AND KEYWORDS** list and **CODE BLUEPRINT**.

For lexical decisions, use the following order:

1. The revised lexical elements and grammar.
2. The consolidated reserved-word list in the project documentation.
3. The later code blueprint and its examples.
4. Earlier wording only where it has not been superseded and remains compatible.
5. Explicit team decisions recorded in this document.

### Team decisions incorporated into this working specification

- Built-in names such as `print`, `play`, `note`, `silence`, `repeat`, `notation`, `data`, `transpose`, and `quarter` are **identifiers**, not reserved words.
- A valid bare note form such as `C4`, `F#3`, or `Bb3` is a **PITCH_LITERAL** wherever it appears.
- Both single-line and multi-line comments are supported.
- String interpolation is part of Kora's syntax and is supported by the lexer. The scanner emits separate string and interpolation tokens, including `STRING_START`, `STRING_TEXT`, `STRING_END`, `INTERPOLATION_START`, and `INTERPOLATION_END`, alongside normal expression tokens. Nested strings and interpolation expressions are supported through a stack of lexer contexts.
- `function` and `fn` are both accepted spellings for a function declaration and are classified as keywords.
- `current` is treated as a keyword even though it is absent from the consolidated reserved-word inventory in the final project documentation.
- `synch` is the reserved spelling. `sync` and `synchronize` are identifiers.
- `st` and `oct` are the only interval-unit tokens. `bt`, `octave`, `semitone`, and `semitones` are identifiers.
- Kora strings are single-line. A physical line break before the closing quotation mark is a lexical error.
- The working string escape set contains seven sequences: `\n`, `\t`, `\r`, `\\`, `\"`, `\{`, and `\'`. Unsupported escape sequences produce lexical errors.
- LF and CRLF are valid source line endings. CRLF counts as one logical newline. A standalone CR is a lexical error.
- Spaces and horizontal tabs are ignored between tokens. Newlines are retained as `NEWLINE` tokens.

These are working lexer decisions. Where the project documentation and a team decision differ, the difference is identified in the relevant section rather than silently hidden.

---

## 2. What the lexer is responsible for

The lexer reads source code from left to right and produces a sequence of tokens. Each token should retain:

- `TokenType` — the category of the token.
- `Lexeme` — the exact source characters that formed the token.
- `LiteralValue` — an optional converted value, such as an integer, float, or Boolean.
- `Line` — the source line where the token starts.
- `Column` — the source column where the token starts.

The lexer does **not** decide whether a program is logically correct. The parser and later semantic checks handle matters such as:

- whether `break` is inside a `for` or `while` loop;
- whether `current` and `|>` are used in the permitted function-array context;
- whether `const` is reassigned;
- whether a pitch or setting is within its allowed range;
- whether an operation is type-compatible;
- whether the program has exactly one valid `class Main` entry point.

The lexer only recognizes the words, literals, operators, punctuation, and boundaries needed by those later stages.

---

## 3. Character set

### 3.1 Letters and digits

```text
Letter = A-Z | a-z
Digit  = 0-9
```

Identifiers are case-sensitive. For example, `Song1` and `song1` are different lexemes.

### 3.2 Characters used by the revised language

The scanner must account for at least these characters because they occur in the revised grammar and code blueprint:

```text
Letters:       A-Z a-z
Digits:        0-9
Whitespace:    space, horizontal tab, LF, CRLF
Punctuation:   ( ) { } [ ] , : ; .
Operators:     + - * / % = < > ! & | #
Comments:      ~
String marker: "
Identifier:    _
```

The `&` and `|` characters are included because the revised logical operators are `&&` and `||`, and the pipeline operator is `|>`.

A carriage return is accepted only as the first character of a CRLF line ending. A standalone CR is not a valid whitespace character and produces a lexical error.

---

## 4. Whitespace and line boundaries

- Ignore ordinary spaces between tokens.
- Ignore horizontal tabs between tokens.
- Preserve source line and column information while scanning.
- Emit a `NEWLINE` token for each physical line break. Newlines may separate statements, while `;` is also a statement terminator and is used inside `for` loop headers.
- Emit a `SEMICOLON` token for `;`.
- Do not remove spaces or tabs inside a quoted string; they are part of its value.
- Treat LF (`\n`) as one logical newline.
- Treat CRLF (`\r\n`) as one logical newline, not two separate newlines.
- Treat a standalone CR (`\r` not followed by LF) as a lexical error.

Recommended token types:

```text
NEWLINE
SEMICOLON
```

When a CRLF pair is consumed, increment the line number once and reset the column once. Do not count the CR and LF as two separate line breaks.

The lexer currently retains `NEWLINE` tokens because the language material identifies newlines as possible statement terminators and the implementation's output uses them. If the parser later decides that newlines are insignificant, remove `NEWLINE` deliberately and update the parser grammar at the same time.

---

## 5. Identifiers

### 5.1 Recognition rule

An identifier:

- starts with an uppercase letter, lowercase letter, or underscore;
- continues with zero or more letters, digits, or underscores;
- is case-sensitive.

```text
Identifier = (Letter | _)(Letter | Digit | _)*
```

Examples:

```kora
Song1
song1
repeat_count
bpm
level
bar
print
play
note
silence
repeat
notation
data
transpose
quarter
whole
half
eighth
sixteenth
```

### 5.2 Keyword lookup

The scanner must read the **entire identifier first** and then check whether the complete lexeme is in the reserved-word list.

For example, `forEach` must be one `IDENTIFIER`, not `for` followed by `Each`.

### 5.3 Built-in names

The team decision is that built-in function names and duration names are ordinary `IDENTIFIER` tokens unless they are explicitly included in the working reserved-word list.

The lexer does not need separate token types for `print`, `play`, `note`, `silence`, `repeat`, `notation`, `data`, `transpose`, `quarter`, `whole`, `half`, `eighth`, or `sixteenth` under this decision.

### 5.4 Noise words

The documentation lists `bpm`, `level`, and `bar` as optional written-out unit words after `tempo`, `volume`, and `beat`. They remain legal names elsewhere. Emit them as `IDENTIFIER` tokens. Let the parser or setting-specific grammar interpret them in the appropriate context.

The words `constant`, `synchronize`, `octave`, `semitone`, and `semitones` are also identifiers. They are not aliases for `const`, `synch`, `oct`, or `st`.

---

## 6. Reserved words and keyword classification

The project documentation describes a consolidated 36-word inventory. The working lexer list below reconciles that inventory with the team's explicit decisions:

- `auto` is included based on the final project documentation.
- `current` is treated as a keyword based on the team's decision, although it is absent from the consolidated list in that documentation.
- `fn` is accepted as a keyword spelling alongside `function`, based on the team's confirmation that both forms are valid.

The resulting working lexer inventory contains **38 reserved lexemes**. This count reflects the working implementation list, not a claim that the original project documentation's consolidated list itself contains 38 entries.

| Group | Reserved lexemes |
|---|---|
| Structure | `class`, `struct`, `track`, `verse`, `new` |
| Declarations and functions | `const`, `let`, `auto`, `function`, `fn`, `pure`, `return` |
| Music settings | `tempo`, `volume`, `beat`, `add`, `synch` |
| Control flow | `if`, `else`, `for`, `while`, `break` |
| Error handling | `try`, `catch`, `throw` |
| Boolean literals and special values | `true`, `false`, `current` |
| Types | `int`, `float`, `boolean`, `String`, `pitch`, `interval`, `event`, `pattern` |
| Interval units | `st`, `oct` |

**Total working inventory: 38 reserved lexemes.** Matching is case-sensitive, so `String` is reserved with that exact capitalization. `string` is an `IDENTIFIER`.

The current implementation uses these broad token categories:

- `KEYWORD` for ordinary reserved words, including `current`, `function`, and `fn`;
- `BOOLEAN` for `true` and `false`;
- `TYPE` for the eight type names;
- `INTERVAL_UNIT` for `st` and `oct`.

The exact enum naming may be adapted to the C implementation. A generic token category may carry the original lexeme so the parser or later stages can distinguish individual keywords.

**Spelling decisions:**

- `synch` is reserved. `sync` and `synchronize` are identifiers.
- `function` and `fn` are both reserved spellings.
- `current` is treated as reserved by team decision, notwithstanding its omission from the consolidated list in the source documentation.
- Only `st` and `oct` are interval units. `bt`, `octave`, `semitone`, and `semitones` are identifiers.

---

## 7. Literals

### 7.1 Integer literal

```text
IntegerLiteral = Digit+
```

Examples:

```kora
0
7
42
127
```

The sign is not part of an ordinary integer token. For example, `-12` is normally tokenized as `MINUS` followed by `INTEGER_LITERAL(12)`.

### 7.2 Float literal

```text
FloatLiteral = Digit+ "." Digit+
```

Examples:

```kora
0.75
3.14
10.0
```

A decimal point is part of a float only when it is followed by at least one digit. Otherwise, `.` is the `DOT` punctuation token.

A number ending with a decimal point, such as `12.`, is therefore tokenized as an `INTEGER_LITERAL` followed by a `DOT`.

A number beginning with a decimal point followed by digits, such as `.5`, is considered a malformed numeric literal and produces an `INVALID_NUMBER` error. Leading-dot float notation is not supported.

Exponent notation is not specified in the Kora material and is not included in this working specification.

### 7.3 Boolean literals

```text
true
false
```

These are reserved words and literal values. The implementation may use a shared `BOOLEAN` token type, with `true` mapped to `true` and `false` mapped to `false` in `LiteralValue`.

### 7.4 Pitch literal

The revised lexical elements define a pitch as:

```text
PitchLiteral = Note Accidental? Octave
Note         = A | B | C | D | E | F | G
Accidental   = # | b
Octave       = -? Digit
```

Examples:

```kora
C4
E4
G4
F#3
Bb3
C-1
G9
```

**Team decision:** A valid bare note form is a `PITCH_LITERAL` wherever it appears. It does not need a preceding `pitch` word. This follows the later revised lexical elements and the final code blueprint, which use bare pitches in assignments, arrays, and arithmetic expressions.

The lexer should recognize a complete pitch-shaped lexeme before falling back to a normal identifier. Do not split `C-1` into `IDENTIFIER(C)`, `MINUS`, and `INTEGER_LITERAL(1)` when it matches the pitch rule.

The documented allowed pitch range, C-1 through G9, is a range/semantic validation rule. The lexer recognizes the pitch's shape; a later stage validates whether its value is allowed.

**Important boundary rule:** A longer identifier-like sequence such as `C4Note` should remain one `IDENTIFIER`, not be split into `PITCH_LITERAL(C4)` plus `IDENTIFIER(Note)`. Require a valid token boundary after a candidate pitch.

### 7.5 Interval values and units

Kora represents intervals using a number followed by a unit:

```text
Interval = (IntegerLiteral | FloatLiteral) Unit
Unit     = st | oct
```

Examples:

```kora
2 st
1 oct
2.5 st
```

The lexer emits **two tokens**, because the number and unit are independently defined lexical elements and `st` / `oct` are reserved words:

```text
INTEGER_LITERAL(2)
INTERVAL_UNIT("st")
```

Likewise, `1 oct` becomes `INTEGER_LITERAL(1)` followed by `INTERVAL_UNIT("oct")`. The parser combines these tokens into an interval expression.

### 7.6 String literals and escape sequences

A string starts and ends with a double quotation mark (`"`).

Kora strings are restricted to one physical source line. An actual line break before the closing quotation mark produces a lexical error.

The working lexer specification supports the following seven escape sequences:

| Escape sequence | Meaning |
|---|---|
| `\n` | Newline character in the string value |
| `\t` | Horizontal tab character in the string value |
| `\r` | Carriage return character in the string value |
| `\\` | Literal backslash |
| `\"` | Literal double quotation mark |
| `\{` | Literal opening brace; does not start interpolation |
| `\'` | Literal single quotation mark |

Only the escape sequences listed above are supported. An unsupported escape sequence produces a lexical error. The error should identify the location of the unsupported escape, preferably the backslash that begins it.

Examples:

Valid:

```kora
print("Hello world")
print("Hello\nworld")
print("She said \"Hello\"")
print("It's a beautiful melody")
print("It\'s a beautiful melody")
print("The symbol \{ is displayed literally")
```

Invalid:

```kora
print("Hello
world")
print("Unsupported escape: \q")
```

Important distinctions:

- `\n` represents a newline character inside the string value. It does not create a physical line break in the source code.
- `\t` and `\r` are escape sequences in the string value; they do not change the source scanner's line position.
- `\"` represents a quotation mark without terminating the string.
- `\{` represents a literal opening brace and must not trigger string interpolation.
- `\'` represents a literal single quotation mark.
- An actual physical line break inside a string is not allowed.
- The lexer must preserve ordinary spaces and characters inside strings.

The scanner must report an unterminated string if the source ends before the closing quotation mark.

**Implementation note:** The seven-sequence rule is the target behavior for the lexer. The current scanner must be updated and tested for `\'` if it does not yet accept it.

### 7.7 String interpolation

String interpolation allows expressions to appear inside strings using braces:

```kora
print("Current count: {count}")
```

The lexer recognizes an unescaped opening brace (`{`) inside a string as the beginning of an interpolation expression. An escaped opening brace (`\{`) remains ordinary string content.

The final interpolation token sequence uses:

| Token type | Purpose |
|---|---|
| `STRING_START` | Opening double quotation mark |
| `STRING_TEXT` | Ordinary string content |
| `INTERPOLATION_START` | Opening brace of an interpolation |
| Normal expression tokens | Tokens inside the interpolation expression |
| `INTERPOLATION_END` | Closing brace of the interpolation |
| `STRING_END` | Closing double quotation mark |

Example:

```kora
print("Calculation: {7 + 3 * 2}")
```

Conceptual token sequence:

```text
IDENTIFIER("print")
LEFT_PAREN
STRING_START
STRING_TEXT("Calculation: ")
INTERPOLATION_START
INTEGER_LITERAL(7)
PLUS
INTEGER_LITERAL(3)
MULTIPLY
INTEGER_LITERAL(2)
INTERPOLATION_END
STRING_END
RIGHT_PAREN
```

When the scanner enters an interpolation expression, it uses the normal token-recognition rules until it encounters the matching closing brace.

The implementation must account for nested braces and strings inside interpolation expressions. For example:

```kora
"Outer {format("Inner {name}")}"
```

This requires the scanner to recognize:

- the outer string;
- the outer interpolation expression;
- a nested string inside the expression;
- a nested interpolation inside that nested string;
- the correct closing boundary for each context.

The scanner must therefore maintain nested string and interpolation contexts instead of relying on a single Boolean flag or one shared brace-depth value for every nested context.

Additional rules:

1. An escaped opening brace (`\{`) is ordinary string content and does not begin interpolation.
2. An unescaped opening brace (`{`) begins interpolation.
3. The matching closing brace (`}`) ends the corresponding interpolation.
4. Braces nested inside an interpolation expression must not prematurely close the outer interpolation.
5. Strings inside interpolation expressions must be scanned using the same string and escape rules.
6. A physical newline before the containing string and its active interpolation contexts are properly closed produces a lexical error.
7. End of source while a string or interpolation context is still open produces a lexical error.

The lexer recognizes the boundaries and tokens of interpolation expressions. Determining whether the expressions are syntactically or semantically valid belongs to the parser and later stages.

---

## 8. Comments

Comments are discarded by the lexer and do not become ordinary output tokens.

### 8.1 Single-line comments

```text
~~ comment text
```

Start when the scanner sees `~~`. Consume characters until the end of the physical line. The newline itself should still be processed according to the newline rule in Section 4.


### 8.2 Multi-line comments

```text
~* comment text
   more comment text *~
```

- Opening marker: `~*`
- Closing marker: `*~`
- Consume and discard all comment text between the markers.
- Update line and column counters for every character, including newlines.
- If the file ends before `*~`, report an unterminated multi-line comment.
- Nested multi-line comments are not supported in this version.

**Newline handling inside multi-line comments:**

Physical line breaks inside a multi-line comment are processed according to the current lexer context.

When no interpolation context is active:

- LF (`\n`) produces one `NEWLINE` token.
- CRLF (`\r\n`) produces one `NEWLINE` token, not two.
- A standalone CR produces a lexical error.

When a physical line break occurs while an interpolation expression is active, including while the scanner is processing a block comment:

- LF produces an `UNTERMINATED_INTERPOLATION` error.
- CRLF produces one `UNTERMINATED_INTERPOLATION` error.
- A standalone CR produces an `INVALID_NEWLINE` error while also terminating the active interpolation context.

After reporting the error, the lexer continues processing the remainder of the block comment. Comment text must not produce ordinary tokens.

This ensures that block-comment handling respects the single-line restriction on string interpolation while preserving normal comment behavior outside interpolation.

Line and column tracking must remain correct throughout the comment.


---

## 9. Operators

The revised grammar and code blueprint use the following operators.

### 9.1 Assignment operators

```text
=   +=   -=   *=   /=   %=
```

### 9.2 Increment and decrement

```text
++   --
```

### 9.3 Arithmetic operators

```text
+   -   *   /   %   **
```

### 9.4 Comparison operators

```text
==   !=   <   >   <=   >=
```

### 9.5 Logical operators

```text
!   &&   ||
```

### 9.6 Function and pipeline operators

```text
|>   =>   ->
```

- `|>` is the pipeline operator.
- `=>` appears in short/lambda function syntax.
- `->` appears in function return-type syntax.

### 9.7 Longest-match rule

When multiple operators begin with the same character, recognize the longest valid operator first. Examples:

- `**` before `*`
- `++` before `+`
- `+=` before `+`
- `==` before `=`
- `!=` before `!`
- `<=` before `<`
- `>=` before `>`
- `&&` and `||` as two-character operators
- `|>` before treating `|` as an invalid standalone character
- `=>` and `->` as complete two-character operators

The lexer does not implement operator precedence. Precedence belongs to the parser.

---

## 10. Delimiters and punctuation

| Lexeme | Suggested token type | Common role |
|---|---|---|
| `(` | `LEFT_PAREN` | Calls, parameters, grouping |
| `)` | `RIGHT_PAREN` | Calls, parameters, grouping |
| `{` | `LEFT_BRACE` | Blocks, struct literals, and interpolation start when in string context |
| `}` | `RIGHT_BRACE` | Blocks, struct literals, and interpolation end when in interpolation context |
| `[` | `LEFT_BRACKET` | Array literals and indexing |
| `]` | `RIGHT_BRACKET` | Array literals and indexing |
| `,` | `COMMA` | Separates list items and arguments |
| `:` | `COLON` | Separates names from types/field values |
| `.` | `DOT` | Member/property access; also used in float literals |
| `;` | `SEMICOLON` | Statement terminator and `for` loop separator |

The meaning of `{` and `}` depends on the current lexer context. Outside strings and interpolation expressions, they are ordinary block or struct delimiters. Inside a string, an unescaped `{` begins interpolation. Inside an interpolation expression, the matching `}` ends that interpolation, while nested braces are tracked normally.

---

## 11. Recommended token inventory

The exact enum naming can be adapted to the implementation language, but the scanner should be able to represent at least these categories:

| Category | Suggested token types |
|---|---|
| Special | `EOF`, `NEWLINE`, `ERROR` |
| Identifier | `IDENTIFIER` |
| Literals | `INTEGER_LITERAL`, `FLOAT_LITERAL`, `BOOLEAN`, `PITCH_LITERAL` |
| Strings and interpolation | `STRING_START`, `STRING_TEXT`, `INTERPOLATION_START`, `INTERPOLATION_END`, `STRING_END` |
| Reserved words | `KEYWORD`, `TYPE`, `INTERVAL_UNIT` (carrying the original lexeme) |
| Assignment | `EQUAL`, `PLUS_EQUAL`, `MINUS_EQUAL`, `STAR_EQUAL`, `SLASH_EQUAL`, `PERCENT_EQUAL` |
| Increment/decrement | `PLUS_PLUS`, `MINUS_MINUS` |
| Arithmetic | `PLUS`, `MINUS`, `STAR`, `SLASH`, `PERCENT`, `POWER` |
| Comparison | `EQUAL_EQUAL`, `BANG_EQUAL`, `LESS`, `GREATER`, `LESS_EQUAL`, `GREATER_EQUAL` |
| Logical | `BANG`, `AND_AND`, `OR_OR` |
| Function/pipeline | `PIPELINE`, `FAT_ARROW`, `ARROW` |
| Punctuation | `LEFT_PAREN`, `RIGHT_PAREN`, `LEFT_BRACE`, `RIGHT_BRACE`, `LEFT_BRACKET`, `RIGHT_BRACKET`, `COMMA`, `COLON`, `DOT`, `SEMICOLON` |

The final interpolation implementation does **not** emit one `STRING_LITERAL` token for an interpolated string. It emits the string and interpolation tokens listed above. A string with no interpolation may still produce `STRING_START`, `STRING_TEXT`, and `STRING_END`.

Comments and ordinary spaces/tabs are skipped rather than emitted as tokens. Newlines are retained as `NEWLINE` tokens according to Section 4.

---

## 12. Scanner recognition order

A hand-coded scanner should follow a predictable order. A practical order for the current interpolation-aware scanner is:

1. Capture the token's starting line and column.
2. If the scanner is currently reading string content, process string characters and escapes before treating comment markers or other symbols as tokens.
3. Outside string content, skip ordinary spaces and horizontal tabs.
4. Recognize LF and CRLF line endings. Outside a string/interpolation context, emit `NEWLINE`; inside a string or interpolation context, report a physical-newline-in-string lexical error. Report a lexical error for standalone CR.
5. Check for `~~` and `~*` comment openers before treating `~` as an unsupported character.
6. If a double quote begins a string, enter a string context and emit `STRING_START`.
7. Within a string context, recognize supported escapes, ordinary text, unescaped `{` as `INTERPOLATION_START`, and the closing quote as `STRING_END`.
8. Within an interpolation context, use normal token recognition. Track nested braces so only the matching `}` closes the current interpolation.
9. Check for a digit and scan an integer or float.
10. Check for a possible pitch form before falling back to a normal identifier.
11. If the character can start an identifier, scan the complete identifier, then perform reserved-word lookup.
12. Check operators using longest-match recognition.
13. Check delimiters and punctuation.
14. If no rule matches, record a lexical error with line and column, consume the offending character, and continue where possible.
15. At the end of the source, report any unterminated string/interpolation/comment context and emit `EOF` according to the implementation's error-reporting design.

The exact order of pitch and identifier scanning may be implemented with a lookahead helper. The key rule is that a complete pitch pattern such as `C4` is classified as `PITCH_LITERAL`, while a longer identifier such as `C4Name` remains one `IDENTIFIER`.

The scanner's string/interpolation context stack must preserve the correct parent context when nested strings and interpolations are entered and exited.

---


## 13. Lexical Errors and Reporting

The scanner must detect and report invalid lexical elements instead of silently accepting them.

At minimum, the scanner should handle the following error categories:

| Error category | Description | Example |
|---|---|---|
| Unexpected character | A character that does not belong to a valid Kora token. | `@` |
| Invalid numeric literal | A malformed integer or floating-point sequence. | `12.3.4` |
| Invalid pitch literal | A pitch-like sequence that does not follow the pitch notation rules. | `C#` |
| Invalid escape sequence | An unsupported escape inside a string. | `\q` |
| Unterminated string | A string that reaches a physical line break or end of source without a closing quotation mark. | `"Hello` |
| Unterminated interpolation | An interpolation expression without its matching closing brace. | `"Hello {name` |
| Unterminated block comment | A multi-line comment without its closing marker. | `~* comment` |
| Invalid newline | A standalone carriage return or unsupported line-ending sequence. | Standalone `\r` |
| Context-stack overflow | The lexer exceeds its supported nested context depth. | Excessive nested contexts |

### 13.1 Error Information

Each reported lexical error should provide the following information when available:

- **Description:** A readable explanation of the detected problem.
- **Lexeme:** The invalid character or source sequence associated with the error.
- **Location:** The source line and column where the lexer detected the error.
- **Suggestion:** A possible correction based on the error category.

Example:

```text
LEXICAL ERROR
Description : Unexpected character.
Lexeme      : '@'
Location    : Line 4, Column 21
Suggestion  : Remove the character or replace it with a valid Kora token.
```

### 13.2 Suggested Corrections

Suggestions should be specific to the detected error whenever possible.

| Error | Example suggestion |
|---|---|
| Unexpected character | Remove the character or replace it with a valid Kora token. |
| Invalid numeric literal | Check the numeric format. Use a valid integer or decimal. |
| Invalid pitch literal | Check the pitch notation and provide a valid note and octave. |
| Invalid escape sequence | Use a supported escape sequence such as `\n`, `\t`, or `\\`. |
| Unterminated string | Add a closing double quotation mark (`"`). |
| Unterminated interpolation | Close the interpolation expression using `}`. |
| Unterminated block comment | Close the block comment using `*~`. |

Suggestions are informational guidance. They do not guarantee that the resulting source code is syntactically or semantically valid. The lexer does not automatically repair source code.

### 13.3 Error Recovery

The lexer should attempt to continue scanning after recoverable lexical errors.

- Consume the offending character or malformed sequence as appropriate.
- Avoid repeatedly reporting the same unconsumed input.
- Preserve line and column tracking.
- Continue recognizing subsequent valid tokens whenever recovery is possible.
- Report multiple lexical errors during one analysis when possible.

Some errors, such as an unterminated block comment at the end of the source file, may prevent further token recognition because the remaining input belongs to the incomplete construct.

### 13.4 Scope of Lexical Error Handling

Lexical error handling is limited to invalid characters, malformed lexemes, and incomplete lexical constructs.

The lexer does not detect:

- Missing or misplaced grammar elements.
- Invalid statement structures.
- Incorrect operator precedence.
- Undeclared variables.
- Type mismatches.
- Invalid program entry points.
- Other semantic violations.

These belong to the parser and subsequent compiler stages.

---

## 14. Suggested test cases for the specification

Use these to test the scanner's token recognition and error handling.

| Input | Expected lexical result |
|---|---|
| `let count = 42` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `INTEGER_LITERAL` |
| `let ratio = 0.75` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `FLOAT_LITERAL` |
| `let root = C4` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let note = F#3` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let low = C-1` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let step = 2 st` | `KEYWORD(let)`, `IDENTIFIER`, `EQUAL`, `INTEGER_LITERAL`, `INTERVAL_UNIT(st)` |
| `count += 2` | `IDENTIFIER`, `PLUS_EQUAL`, `INTEGER_LITERAL` |
| `count++` | `IDENTIFIER`, `PLUS_PLUS` |
| `a >= 3 && !(b < 5)` | `IDENTIFIER`, `GREATER_EQUAL`, `INTEGER_LITERAL`, `AND_AND`, `BANG`, ... |
| `operations[0 |> 1 |> 0]` | `IDENTIFIER`, brackets, integer literals, `PIPELINE` tokens |
| `~~ comment` | No comment token; newline handled separately |
| `~* comment *~` | No comment token |
| `print("Hello {name}")` | `IDENTIFIER`, `LEFT_PAREN`, `STRING_START`, `STRING_TEXT`, `INTERPOLATION_START`, `IDENTIFIER(name)`, `INTERPOLATION_END`, `STRING_END`, `RIGHT_PAREN` |
| `forEach` | One `IDENTIFIER`, not `KEYWORD(for)` plus another token |
| `current Current` | `KEYWORD(current)`, `IDENTIFIER(Current)` |
| `function fn` | `KEYWORD(function)`, `KEYWORD(fn)` |
| `synch sync synchronize` | `KEYWORD(synch)`, `IDENTIFIER(sync)`, `IDENTIFIER(synchronize)` |
| `oct st bt octave semitone semitones` | `INTERVAL_UNIT(oct)`, `INTERVAL_UNIT(st)`, then four `IDENTIFIER` tokens |
| `print("Hello\nworld")` | A string containing the supported `\n` escape; no physical source newline |
| `print("She said \"Hello\"")` | A string containing escaped double quotation marks |
| `print("It\'s fine")` | A string containing an escaped single quotation mark |
| `print("\{name}")` | String text containing literal `{name}`; no interpolation tokens |
| `"Outer {format("Inner {name}")}"` | Correctly nested string and interpolation tokens for both levels |

Also test source line endings:

- LF (`\n`) produces one `NEWLINE` token.
- CRLF (`\r\n`) produces one `NEWLINE` token, not two.
- Standalone CR produces a lexical error.
- Spaces and horizontal tabs between tokens are skipped.

Test malformed inputs:

- a missing closing quote;
- a physical line break inside a string;
- an unsupported escape such as `\q`;
- an interpolation expression missing its closing `}`;
- a missing `*~` comment terminator;
- an unsupported character;
- a malformed pitch-like sequence;
- a standalone CR.


### 14.1 Lexical Error Reporting Tests

The following inputs test whether the scanner detects errors and provides useful diagnostic information.

| Input | Expected error |
|---|---|
| `int score = 100 @` | Unexpected character |
| `float price = 12.3.4` | Invalid numeric literal |
| `pitch note = C#` | Invalid pitch literal |
| `"Hello, Kora!\q"` | Invalid escape sequence |
| `"This string is never closed` | Unterminated string |
| `"Hello {name` | Unterminated interpolation |
| `~* This block comment is never closed` | Unterminated block comment |

For each error, verify that the output includes:

1. An appropriate error description.
2. The associated invalid lexeme or source sequence.
3. The source line and column.
4. A relevant correction suggestion.

Also verify that the lexer continues scanning after recoverable errors and reports multiple errors in a single source file.

Additional tests should cover standalone carriage returns, excessive nested contexts, and multiple errors on the same line.

---

## 15. Remaining implementation considerations


## 15. Remaining implementation decisions

The following decisions have been established for the current Kora lexical specification:

1. **Tabs:** Horizontal tabs are treated as whitespace outside strings.
2. **String escapes:** The seven supported escape sequences are `\n`, `\t`, `\r`, `\\`, `\"`, `\{`, and `\'`. Unsupported escape sequences produce lexical errors.
3. **Multiline strings:** Physical line breaks inside strings are not allowed.
4. **Source line endings:** LF and CRLF are supported. CRLF counts as one logical newline. A standalone CR produces a lexical error.
5. **Synchronization spelling:** Only `synch` is reserved. `sync` and `synchronize` are identifiers.
6. **Interval units:** Only `st` and `oct` are interval units. `bt`, `octave`, `semitone`, and `semitones` are identifiers.
7. **Function declaration spelling:** Both `function` and `fn` are accepted as keywords.
8. **Current keyword:** `current` is treated as a keyword by team decision.
9. **String interpolation:** Nested strings and interpolation expressions are supported through a stack of lexer contexts.
10. **Nested block comments:** Nested multi-line comments are not supported.
11. **Newlines inside multi-line comments:** Physical line breaks inside discarded comments are still recognized and emitted as `NEWLINE` tokens. LF produces one token, and CRLF produces one token.

These are the working language and implementation decisions for the current lexer. Any future change must be reflected in this document and tested against the scanner implementation.


---

## 16. Completion criteria for Step 1

Step 1 is complete when the team agrees that this document accurately records:

- the character and whitespace rules, including LF, CRLF, and standalone CR;
- the identifier rule and case sensitivity;
- the working reserved-word inventory and the documented differences from the source list;
- the literal forms, including pitch and interval forms;
- both comment styles and the nested-comment decision;
- the complete operator and delimiter inventory;
- the seven string escape sequences and single-line string rule;
- the implemented string/interpolation token behavior, including nested contexts;
- the token categories and lexical error expectations;
- the remaining newline-token consideration for multi-line comments.
- lexical error categories and their corresponding descriptions;
- source locations and associated invalid lexemes;
- rule-based correction suggestions;
- error recovery behavior and continued scanning after recoverable errors;
- the distinction between lexical errors and syntax or semantic errors.

After the specification is agreed, the team can continue refining the `TokenType` inventory, token data structure, scanner implementation, and test suite. Implementation changes should be tested against this document and updated here when an explicit language decision changes.
