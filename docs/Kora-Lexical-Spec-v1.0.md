# Kora Lexical Specification v1.0

**Status:** Working specification for the Kora hand-coded lexical analyzer

**Purpose:** Define how the lexer converts Kora source characters into tokens.

**Implementation constraint:** Recognition must be implemented manually with a character scanner. Standard language data structures (for example, lists, dictionaries, and enums) are allowed; lexer generators, parser generators, regex-based tokenizers, and compiler frameworks are out of scope.

---

## 1. Source and revision authority

Use the established Kora specification in the first part of the documentation, including its first revision, as the baseline. Apply the additional material under **MORE REVISIONS**, followed by the consolidated **RESERVED WORDS AND KEYWORDS** list and **CODE BLUEPRINT**.

For lexical decisions, use the following order:

1. The revised lexical elements and grammar.
2. The consolidated 36-word reserved-word list.
3. The later code blueprint and its examples.
4. Earlier wording only where it has not been superseded and remains compatible.
5. Explicit team decisions recorded in this document.

### Team decisions incorporated into v1.0

- Built-in names such as `print`, `play`, `note`, `silence`, `repeat`, `notation`, `data`, `transpose`, and `quarter` are **identifiers**, not reserved words.
- A valid bare note form such as `C4`, `F#3`, or `Bb3` is a **PITCH_LITERAL** wherever it appears.
- Both single-line and multi-line comments are supported.
- String interpolation is part of Kora's intended syntax. It will be implemented in two stages: first, recognize a complete quoted string as one `STRING_LITERAL`; later, extend scanning to emit separate text and interpolation-expression tokens.

---

## 2. What the lexer is responsible for

The lexer reads source code from left to right and produces a sequence of tokens. Each token should retain:

- `TokenType` — the category of the token.
- `Lexeme` — the exact source characters that formed the token.
- `LiteralValue` — an optional converted value, such as an integer or float.
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
Whitespace:    space, newline
Punctuation:   ( ) { } [ ] , : ; .
Operators:     + - * / % = < > ! & | #
Comments:      ~
String marker: "
Identifier:    _
```

The `&` and `|` characters are included because the revised logical operators are `&&` and `||`, and the pipeline operator is `|>`.

**Implementation note:** The documentation explicitly identifies spaces and newlines. Tab handling is not fully specified. Recommended behavior is to treat a horizontal tab as whitespace, while preserving line breaks as `NEWLINE` tokens if the parser uses line breaks to separate statements.

---

## 4. Whitespace and line boundaries

- Ignore ordinary spaces between tokens.
- Preserve source line and column information while scanning.
- Emit a `NEWLINE` token for a physical line break if the parser will use newlines as statement separators. The earlier grammar identifies newline or `;` as a statement terminator, and the revised code examples commonly place statements on separate lines.
- Emit a `SEMICOLON` token for `;`. Semicolons are also used inside `for` loop headers.
- Do not remove spaces inside a quoted string; they are part of its value.
- Handle Windows CRLF (`\r\n`) as one logical newline, not two separate newlines.

Recommended token types:

```text
NEWLINE
SEMICOLON
```

If the team later decides that newlines are always insignificant, remove `NEWLINE` from the token stream deliberately and update the parser grammar at the same time. Do not silently discard them before that decision is made.

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
```

### 5.2 Keyword lookup

The scanner should read the **entire identifier first** and then check whether the complete lexeme is in the reserved-word list.

For example, `forEach` must be one `IDENTIFIER`, not `for` followed by `Each`.

### 5.3 Built-in names

The team decision is that built-in function names and duration names are ordinary `IDENTIFIER` tokens unless they appear in the official 36-word reserved-word list.

The lexer does not need separate token types for `print`, `play`, `note`, `silence`, `repeat`, `notation`, `data`, `transpose`, `quarter`, `whole`, `half`, `eighth`, or `sixteenth` under this decision.

### 5.4 Noise words

The documentation lists `bpm`, `level`, and `bar` as optional written-out unit words after `tempo`, `volume`, and `beat`. They remain legal names elsewhere. For the initial hand-coded lexer, emit them as `IDENTIFIER` tokens. Let the parser or setting-specific grammar interpret them in the appropriate context.

---

## 6. Reserved words

The consolidated final list contains 36 reserved words:

| Group | Reserved words |
|---|---|
| Structure | `class`, `struct`, `track`, `verse`, `new` |
| Declarations | `const`, `let`, `function`, `pure`, `return` |
| Music settings | `tempo`, `volume`, `beat`, `add`, `synch` |
| Control flow | `if`, `else`, `for`, `while`, `break` |
| Errors | `try`, `catch`, `throw` |
| Boolean literals / special | `true`, `false`, `current` |
| Types | `int`, `float`, `boolean`, `String`, `pitch`, `interval`, `event`, `pattern` |
| Units | `st`, `oct` |

**Total: 36.** Matching is case-sensitive, so `String` is reserved with that exact capitalization.

Recommended token representation:

- Either use one generic `KEYWORD` token with the original lexeme, or
- use dedicated token types such as `KW_CLASS`, `KW_LET`, `KW_IF`, `KW_ST`, etc.

For a student lexer, dedicated token types make parser implementation and token-output demonstrations easier to read. `true` and `false` may use a shared `BOOLEAN_LITERAL` token type with a Boolean `LiteralValue`.

**Spelling note:** The consolidated list and later code blueprint use `synch`. The earlier material also contains `sync`. In v1.0, reserve `synch`; do not silently add `sync` as a second keyword. The team can explicitly restore `sync` as an alias later if required.

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

A decimal point is part of a float only when it is followed by at least one digit. Otherwise, `.` is the `DOT` punctuation token. Exponent notation is not specified in the Kora material and is not included in v1.0.

### 7.3 Boolean literals

```text
true
false
```

These are reserved words and literal values. Recommended token type: `BOOLEAN_LITERAL`, with `true` mapped to `true` and `false` mapped to `false` in `LiteralValue`.

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

The lexer should recognize a complete pitch-shaped lexeme before falling back to `IDENTIFIER`. Do not split `C-1` into `IDENTIFIER(C)`, `MINUS`, and `INTEGER_LITERAL(1)` when it matches the pitch rule.

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

Recommended lexer output is **two tokens**, because the number and unit are independently defined lexical elements and `st` / `oct` are reserved words:

```text
INTEGER_LITERAL(2)
KW_ST("st")
```

Likewise, `1 oct` becomes `INTEGER_LITERAL(1)` followed by `KW_OCT("oct")`. The parser combines these tokens into an interval expression.

### 7.6 String literal — first implementation stage

A string starts and ends with a double quote (`"`). The revised grammar allows ordinary text and interpolation expressions of the form `{expr}` inside the string.

For the first implementation milestone, scan the complete quoted sequence as one token:

```text
STRING_LITERAL
```

Example:

```kora
print("Current count: {count}")
```

Initial output:

```text
IDENTIFIER("print")
LEFT_PAREN("(")
STRING_LITERAL("Current count: {count}")
RIGHT_PAREN(")")
```

At this stage, `{count}` remains part of the string lexeme. Do not attempt to tokenize `count` separately yet. This lets the team finish and test ordinary string-boundary handling before adding the more complex interpolation scanner.

The first-stage string scanner must:

1. Consume the opening `"`.
2. Read until the matching closing `"`.
3. Preserve all characters inside the string, including spaces and braces.
4. Report an unterminated string if the source ends before the closing quote.
5. Track line and column positions correctly.

Escape sequences (for example, `\"` or `\\`) are not clearly specified in the supplied language rules. Do not invent escape behavior without agreeing on it. A simple initial implementation may reject an unescaped quote inside a string and may disallow physical newlines inside strings until the team defines those cases.

### 7.7 String interpolation — final intended extension

After ordinary strings are working, extend the scanner so an interpolated string can produce a sequence such as:

```kora
print("Calculation: {7 + 3 * 2}")
```

Conceptual final tokens:

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

When the scanner enters an interpolation expression, it reuses the normal token-recognition rules until it reaches the matching `}`. It then returns to string mode. The eventual implementation must account for nested braces and strings inside expressions, not merely stop at the first `}` unconditionally.

This is a later milestone. It is not required to make the first `STRING_LITERAL` scanner work.

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
- Consume everything between the markers as comment content.
- Update line and column counters for every character, including newlines.
- If the file ends before `*~`, report an unterminated multi-line comment.

Nested multi-line comments are not specified. Do not support nesting in the first version unless the team explicitly adds that rule.

The scanner must check the two-character comment markers before treating `~` as an invalid standalone character.

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
| `{` | `LEFT_BRACE` | Blocks, struct literals, interpolation boundary |
| `}` | `RIGHT_BRACE` | Blocks, struct literals, interpolation boundary |
| `[` | `LEFT_BRACKET` | Array literals and indexing |
| `]` | `RIGHT_BRACKET` | Array literals and indexing |
| `,` | `COMMA` | Separates list items and arguments |
| `:` | `COLON` | Separates names from types/field values |
| `.` | `DOT` | Member/property access; also used in float literals |
| `;` | `SEMICOLON` | Statement terminator and `for` loop separator |

For stage 1 string scanning, braces inside a quoted string are just string content. For the later interpolation stage, the scanner may use dedicated `INTERPOLATION_START` and `INTERPOLATION_END` token types to distinguish interpolation braces from ordinary block braces.

---

## 11. Recommended token inventory

The exact enum naming can be adapted to the implementation language, but the scanner should be able to represent at least these categories:

| Category | Suggested token types |
|---|---|
| Special | `EOF`, `NEWLINE` |
| Identifier | `IDENTIFIER` |
| Literals | `INTEGER_LITERAL`, `FLOAT_LITERAL`, `BOOLEAN_LITERAL`, `PITCH_LITERAL`, `STRING_LITERAL` |
| Reserved words | Dedicated `KW_*` types for the 36 reserved words, or one `KEYWORD` type carrying the lexeme |
| Assignment | `EQUAL`, `PLUS_EQUAL`, `MINUS_EQUAL`, `STAR_EQUAL`, `SLASH_EQUAL`, `PERCENT_EQUAL` |
| Increment/decrement | `PLUS_PLUS`, `MINUS_MINUS` |
| Arithmetic | `PLUS`, `MINUS`, `STAR`, `SLASH`, `PERCENT`, `POWER` |
| Comparison | `EQUAL_EQUAL`, `BANG_EQUAL`, `LESS`, `GREATER`, `LESS_EQUAL`, `GREATER_EQUAL` |
| Logical | `BANG`, `AND_AND`, `OR_OR` |
| Function/pipeline | `PIPELINE`, `FAT_ARROW`, `ARROW` |
| Punctuation | `LEFT_PAREN`, `RIGHT_PAREN`, `LEFT_BRACE`, `RIGHT_BRACE`, `LEFT_BRACKET`, `RIGHT_BRACKET`, `COMMA`, `COLON`, `DOT`, `SEMICOLON` |
| Later interpolation extension | `STRING_START`, `STRING_TEXT`, `INTERPOLATION_START`, `INTERPOLATION_END`, `STRING_END` |

Comments and ordinary spaces are skipped rather than emitted as tokens. Newlines are retained if the parser uses them as statement boundaries.

---

## 12. Scanner recognition order

A hand-coded scanner should follow a predictable order. A practical initial order is:

1. Capture the token's starting line and column.
2. Skip ordinary spaces and other agreed horizontal whitespace.
3. If a newline is encountered, update position and emit `NEWLINE` if required.
4. Check for `~~` and `~*` comment openers before handling other symbols.
5. Check for a double quote and call the stage 1 string scanner.
6. Check for a digit and scan an integer or float.
7. Check for a possible pitch form before falling back to a normal identifier.
8. If the character can start an identifier, scan the complete identifier, then perform reserved-word lookup.
9. Check operators using longest-match recognition.
10. Check delimiters and punctuation.
11. If no rule matches, record a lexical error with line and column, consume the offending character, and continue where possible.
12. At the end of the source, emit `EOF`.

The exact order of pitch and identifier scanning may be implemented with a lookahead helper. The key rule is that a complete pitch pattern such as `C4` is classified as `PITCH_LITERAL`, while a longer identifier such as `C4Name` remains one `IDENTIFIER`.

---

## 13. Lexical errors to handle

At minimum, the scanner should report:

- Unterminated string literal.
- Unterminated multi-line comment.
- Unknown or unsupported character.
- Malformed numeric or pitch sequence when it cannot be tokenized under the agreed rules.

Every error should include the line and column. The scanner should recover when possible by consuming at least one invalid character and continuing, rather than crashing or entering an infinite loop.

Do not report parser or semantic errors as lexical errors. For example, using `break` outside a loop is not a lexical error; `break` is a valid reserved word token.

---

## 14. Suggested test cases for the specification

Use these before testing the full code blueprint.

| Input | Expected lexical result |
|---|---|
| `let count = 42` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `INTEGER_LITERAL` |
| `let ratio = 0.75` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `FLOAT_LITERAL` |
| `let root = C4` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let note = F#3` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let low = C-1` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `PITCH_LITERAL` |
| `let step = 2 st` | `KW_LET`, `IDENTIFIER`, `EQUAL`, `INTEGER_LITERAL`, `KW_ST` |
| `count += 2` | `IDENTIFIER`, `PLUS_EQUAL`, `INTEGER_LITERAL` |
| `count++` | `IDENTIFIER`, `PLUS_PLUS` |
| `a >= 3 && !(b < 5)` | `IDENTIFIER`, `GREATER_EQUAL`, `INTEGER_LITERAL`, `AND_AND`, `BANG`, ... |
| `operations[0 |> 1 |> 0]` | `IDENTIFIER`, brackets, integer literals, `PIPELINE` tokens |
| `~~ comment` | No comment token; newline handled separately |
| `~* comment *~` | No comment token |
| `print("Hello {name}")` | `IDENTIFIER`, `LEFT_PAREN`, one `STRING_LITERAL`, `RIGHT_PAREN` in stage 1 |
| `forEach` | One `IDENTIFIER`, not `KW_FOR` plus another token |

Also test malformed inputs: a missing closing quote, a missing `*~`, an unsupported character, and a malformed pitch-like sequence.

---

## 15. Items to confirm with the team (not blockers for the first scanner milestone)

These details are not fully and consistently defined by the language material. Record the team's choice before implementing the relevant edge case:

1. **Tabs:** Treat `\t` as whitespace (recommended), or reject it.
2. **String escapes:** Decide whether `\"`, `\\`, `\n`, and similar sequences are supported.
3. **Multiline strings:** Decide whether a quoted string may contain a physical newline.
4. **Nested block comments:** v1.0 recommendation is no nested `~* ... *~` comments.
5. **`sync` spelling:** The consolidated reserved-word list uses `synch`; decide whether the earlier `sync` spelling should remain an alias.
6. **Newline tokens inside multiline comments:** Recommended behavior is to preserve physical line boundaries if newlines terminate statements.

These decisions should be written here before the scanner implementation begins to depend on them.

---

## 16. Completion criteria for Step 1

Step 1 is complete when the team agrees that this document accurately records:

- the character and whitespace rules;
- the identifier rule and case sensitivity;
- the final 36 reserved words;
- the literal forms, including pitch and interval forms;
- both comment styles;
- the complete operator and delimiter inventory;
- the initial and eventual string/interpolation behavior;
- the token categories and lexical error expectations;
- the remaining open edge-case decisions.

Only after this specification is agreed should the team proceed to the `TokenType` inventory and token data structure in the chosen implementation language.