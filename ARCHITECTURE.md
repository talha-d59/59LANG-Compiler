# 59LANG Architecture Diagrams

## System Overview

```
╔══════════════════════════════════════════════════════════════════════════╗
║                         59LANG COMPILER SYSTEM                            ║
╚══════════════════════════════════════════════════════════════════════════╝

┌──────────────────────────────────────────────────────────────────────────┐
│                          USER INTERACTION LAYER                           │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌─────────────────────────┐          ┌──────────────────────────┐       │
│  │   Web Browser (3000)    │          │   CLI Terminal           │       │
│  │  ┌──────────────────┐   │          │  compiler program.code   │       │
│  │  │  Code Editor     │   │          │                          │       │
│  │  │  Error Display   │   │          │  JSON Output Format      │       │
│  │  │  Symbol Viewer   │   │          └──────────────────────────┘       │
│  │  └──────────────────┘   │                                              │
│  │         HTML/CSS/JS      │                                              │
│  └──────────┬───────────────┘                                              │
│             │                                                              │
│             │ HTTP POST /api/compile                                       │
│             ↓                                                              │
└──────────────────────────────────────────────────────────────────────────┘
               │
               │
┌──────────────────────────────────────────────────────────────────────────┐
│                         API GATEWAY LAYER (Port 5000)                     │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│        Flask Server (app.py)                                              │
│        ├── /api/health ────────── Health check                            │
│        ├── /api/compile ─────────► Compile endpoint                       │
│        └── /api/examples ─────────  Example programs                      │
│                                                                            │
└──────────────────────┬───────────────────────────────────────────────────┘
                       │
                       │ Execute: ./compiler --json
                       ↓
┌──────────────────────────────────────────────────────────────────────────┐
│                       COMPILER EXECUTION LAYER                            │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  C++ Executable (compiler)                                                │
│                                                                            │
│  Input:  Source code (.code file)                                         │
│  Output: JSON (errors, symbol table, status)                              │
│                                                                            │
└──────────────────────────────────────────────────────────────────────────┘
```

## Compiler Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│                    COMPILATION PIPELINE                          │
└─────────────────────────────────────────────────────────────────┘

PHASE 1: LEXICAL ANALYSIS (Scanner)
═════════════════════════════════════════════════════════════════

Input:  nexus { shard core x = 5; broadcast x; }

Process:
  Character Stream → Tokenization → Token Stream
  
  'n' 'e' 'x' 'u' 's' → Token(NEXUS)
  '{' → Token(LBRACE)
  's' 'h' 'a' 'r' 'd' → Token(SHARD)
  'c' 'o' 'r' 'e' → Token(CORE)
  'x' → Token(IDENTIFIER, "x")
  '=' → Token(ASSIGN)
  '5' → Token(NUMBER, "5")
  ';' → Token(SEMICOLON)
  ... etc

Errors Detected:
  - Illegal characters (e.g., '@', '#')
  - Unterminated strings

Output: Token Stream [Token, Token, Token, ...]


PHASE 2: SYNTAX ANALYSIS (Parser)
═════════════════════════════════════════════════════════════════

Input:  [NEXUS, LBRACE, SHARD, CORE, IDENTIFIER(x), ASSIGN, NUMBER(5), SEMICOLON, ...]

Process: Recursive Descent Parsing

  parseProgram()
    ├─ match(NEXUS) ✓
    ├─ match(LBRACE) ✓
    ├─ parseDeclarations()
    │   └─ parseDeclaration()
    │       ├─ match(SHARD) ✓
    │       ├─ parseType() → CORE
    │       └─ parseIdentifierList() → [x]
    │           → Add to SymbolTable: x → core
    ├─ parseStatements()
    │   └─ parseStatement()
    │       └─ parseFunctionCall() → output
    └─ match(RBRACE) ✓

Errors Detected:
  - Syntax violations (grammar rules)
  - Duplicate variable declarations
  - Undeclared variable usage

Output: AST (Abstract Syntax Tree) + Symbol Table


PHASE 3: SEMANTIC ANALYSIS
═════════════════════════════════════════════════════════════════

During Parsing:
  - Variable declaration → Add to symbol table
  - Variable usage → Check in symbol table
  - Type validation → Verify variable exists

Errors Detected:
  - Undeclared variables: output undefined_var;
  - Duplicate declarations: shard core x; shard core x;
  - Type mismatches (future enhancement)

Output: Final Error List


PHASE 4: OUTPUT GENERATION
═════════════════════════════════════════════════════════════════

Format: JSON

{
  "success": true/false,
  "errors": [
    {
      "message": "Symbol 'x' not declared",
      "line": 8,
      "column": 14,
      "type": "SEMANTIC"
    }
  ],
  "symbolTable": {
    "x": {
      "name": "x",
      "type": "int",
      "line": 2,
      "column": 10
    }
  },
  "errorCount": 0,
  "hasErrors": false
}

Output: JSON string → Display in Frontend
```

## Scanner (Lexer) State Machine

```
┌────────────────────────────────────────────────────────┐
│              SCANNER STATE MACHINE                      │
└────────────────────────────────────────────────────────┘

[START]
  ↓
  Is Whitespace? ───YES──→ Skip ──→ [START]
  ↓ NO
  Is Comment (%)? ──YES──→ Skip to EOL ──→ [START]
  ↓ NO
  Is Letter/Underscore? ─YES──→ Scan Identifier/Keyword ──→ [IDENTIFIER/KEYWORD]
  ↓ NO
  Is Digit? ────────YES──→ Scan Number ──→ [NUMBER/FLOAT]
  ↓ NO
  Is Quote? ────────YES──→ Scan String ──→ [STRING]
  ↓ NO
  Is Operator/Punctuation? ──YES──→ Scan Op ──→ [OPERATOR]
  ↓ NO
  Is EOF? ───────YES──→ [END_OF_FILE]
  ↓ NO
  ERROR: Illegal Character
```

## Parser Grammar (Simplified BNF)

```
program       : MAIN LBRACE declarations statements RBRACE

declarations  : (declaration)*

declaration   : VAR type identifier_list SEMICOLON

identifier_list : IDENTIFIER (COMMA IDENTIFIER)*

type          : INT | FLOAT | BOOL | STRING

statements    : (statement)*

statement     : assignment
              | if_statement
              | while_statement
              | for_statement
              | return_statement
              | input_statement
              | output_statement

assignment    : IDENTIFIER ASSIGN expression SEMICOLON

if_statement  : IF LPAREN condition RPAREN LBRACE statements RBRACE
                (ELSE LBRACE statements RBRACE)?

while_statement : WHILE LPAREN condition RPAREN LBRACE statements RBRACE

for_statement : FOR LPAREN assignment condition SEMICOLON expression RPAREN 
                LBRACE statements RBRACE

expression    : logical_or

logical_or    : logical_and (OR logical_and)*

logical_and   : equality (AND equality)*

equality      : comparison ((EQUAL | NOT_EQUAL) comparison)*

comparison    : addition ((LESS | GREATER | LESS_EQUAL | GREATER_EQUAL) addition)*

addition      : multiplication ((PLUS | MINUS) multiplication)*

multiplication: unary ((MULTIPLY | DIVIDE | MODULO) unary)*

unary         : (NOT | MINUS)? primary

primary       : NUMBER | FLOAT_NUMBER | STRING | IDENTIFIER | LPAREN expression RPAREN
```

## Abstract Syntax Tree (AST) Example

```
Source Code:
─────────────────────────────
nexus {
    shard core x = 5;
    shard core y = 10;
    shard core sum = x + y;
    broadcast sum;
}
─────────────────────────────

AST Representation:
─────────────────────────────

Program("nexus")
├── Declaration(core, [x, y, sum])
│   ├── Identifier("x")
│   ├── Identifier("y")
│   └── Identifier("sum")
│
└── Statements
    ├── Assignment("x", Literal(5))
    ├── Assignment("y", Literal(10))
    ├── Assignment("sum", BinaryOp(
    │   └── left: Identifier("x")
    │   └── op: "+"
    │   └── right: Identifier("y")
    │ ))
    └── FunctionCall("broadcast", [Identifier("sum")])
```

## Symbol Table Example

```
After parsing:  shard core x; shard flux y; shard glyph name;

Symbol Table:
┌─────────────────────────────────────────────────────┐
│ Name  │ Type   │ Line │ Column │ Initialized        │
├─────────────────────────────────────────────────────┤
│ x     │ core   │ 2    │ 10     │ No                 │
│ y     │ flux   │ 3    │ 10     │ No                 │
│ name  │ glyph  │ 4    │ 10     │ No                 │
└─────────────────────────────────────────────────────┘

When accessing variables:
  - "listen x" → Check if "x" exists → FOUND (OK)
  - "broadcast z" → Check if "z" exists → NOT FOUND (ERROR)
```

## Error Detection Flow

```
Error Detection Phases:
══════════════════════════════════════════════════════════════

┌──────────────────┐
│  Source Code     │
└────────┬─────────┘
         │
         ↓
    ┌─────────────────────────────────────┐
    │ SCANNER - Lexical Analysis          │
    ├─────────────────────────────────────┤
    │ Checks:                              │
    │ • Illegal characters (#, @, etc.)   │
    │ • Unterminated strings              │
    │ • Invalid number formats            │
    │                                      │
    │ Error Type: SCANNER                 │
    └────────┬────────────────────────────┘
             │
             ↓
    ┌─────────────────────────────────────┐
    │ PARSER - Syntax Analysis            │
    ├─────────────────────────────────────┤
    │ Checks:                              │
    │ • Grammar rule violations           │
    │ • Missing semicolons                │
    │ • Unmatched braces                  │
    │                                      │
    │ Error Type: PARSER                  │
    └────────┬────────────────────────────┘
             │
             ↓
    ┌─────────────────────────────────────┐
    │ SEMANTIC - Symbol Analysis          │
    ├─────────────────────────────────────┤
    │ Checks:                              │
    │ • Undeclared variables              │
    │ • Duplicate declarations            │
    │ • Type mismatches                   │
    │                                      │
    │ Error Type: SEMANTIC                │
    └────────┬────────────────────────────┘
             │
             ↓
    ┌─────────────────────────────────────┐
    │ Output: Error List (JSON)           │
    └─────────────────────────────────────┘
```

## Frontend Request/Response Cycle

```
USER INTERACTION                    NETWORK                      BACKEND
═════════════════════════════════════════════════════════════════════════

[Editor]
  │ Write code
  │
  ├─ Click "Compile"
  │
  └─→ JavaScript: compileCode()
      │
      ├─ Get code from textarea
      │
      └─→ POST /api/compile
          ├─ Headers: Content-Type: application/json
          │
          └─ Body: {
                "code": "nexus { shard core x; ... }"
             }
             ════════════════════════════════════════════════════════→
                                                  Flask receives request
                                                        │
                                                        ├─ Create temp file
                                                        ├─ Execute compiler
                                                        ├─ Parse JSON output
                                                        └─ Return result
             ←════════════════════════════════════════════════════════
             Response: {
               "success": true,
               "errors": [],
               "symbolTable": { "x": {...} },
               "errorCount": 0,
               "hasErrors": false
             }
      │
      └─→ JavaScript: displayResults(result)
          │
          ├─ showErrors(result.errors)
          ├─ showSymbols(result.symbolTable)
          ├─ updateBadges(result.errorCount)
          └─ switchTab('symbols')

[UI Updates]
  ├─ Error list (red badges)
  ├─ Symbol table (blue cards)
  ├─ Success/failure message
  └─ Tab switches to show results
```

## Data Flow Diagram

```
┌────────────────────────────────────────────────────────────────┐
│                      COMPLETE DATA FLOW                         │
└────────────────────────────────────────────────────────────────┘

1. USER INPUT
   ┌──────────────────────────┐
   │  Write code in editor    │
   │  Click Compile button    │
   └────────────┬─────────────┘
                │ code: string

2. FRONTEND SENDS REQUEST
   ┌────────────────────────────────────────┐
   │ POST http://localhost:5000/api/compile │
   │ Content-Type: application/json         │
   │ Body: {"code": "nexus {...}"}         │
   └────────────┤─────────────────────────┘
                │ HTTP

3. BACKEND RECEIVES
   ┌──────────────────────────────────┐
   │ Flask app receives JSON request  │
   │ Extract code field               │
   │ Write to temporary .code file    │
   └────────────┬─────────────────────┘
                │ file: /tmp/xxx.code

4. COMPILER EXECUTION
   ┌─────────────────────────────────────────────┐
   │ subprocess.run([compiler, file, '--json'])  │
   │                                             │
   │ ├─ Scanner: Tokenization                    │
   │ ├─ Parser: AST Building                     │
   │ └─ Semantic: Symbol Checking                │
   │                                             │
   │ Result: JSON output                         │
   └────────────┬────────────────────────────────┘
                │ stdout: JSON string

5. BACKEND PROCESSES
   ┌──────────────────────────────┐
   │ Parse JSON from compiler     │
   │ Format response              │
   │ Return to frontend           │
   └────────────┬─────────────────┘
                │ HTTP response

6. FRONTEND DISPLAYS
   ┌──────────────────────────────────┐
   │ Parse response JSON              │
   │ Update error display             │
   │ Update symbol table              │
   │ Update badges                    │
   │ Switch to results tab            │
   └──────────────────────────────────┘

7. USER SEES RESULTS
   ┌──────────────────────────────────┐
   │ Error list with line numbers     │
   │ Symbol table with types          │
   │ Compilation status               │
   └──────────────────────────────────┘
```

## Directory Tree with File Types

```
mini-compiler-cpp/
│
├── 📄 CMakeLists.txt              [Build Configuration]
├── 📄 README.md                   [Documentation]
├── 📄 SETUP.md                    [Installation Guide]
├── 📄 LANGUAGE_REFERENCE.md       [Syntax Guide]
├── 📄 ARCHITECTURE.md             [This File]
│
├── 📁 include/                    [C++ Headers]
│   ├── 📄 error.h
│   ├── 📄 token.h
│   ├── 📄 scanner.h
│   ├── 📄 parser.h
│   ├── 📄 ast_node.h
│   └── 📄 symbol_table.h
│
├── 📁 src/                        [C++ Implementation]
│   ├── 📄 main.cpp
│   ├── 📄 scanner.cpp
│   ├── 📄 parser.cpp
│   ├── 📄 token.cpp
│   ├── 📄 ast_node.cpp
│   └── 📄 symbol_table.cpp
│
├── 📁 build/                      [Compiled Output]
│   └── 📦 compiler (or compiler.exe)
│
├── 📁 backend/                    [Python Flask]
│   ├── 📄 app.py
│   └── 📄 requirements.txt
│
├── 📁 frontend/                   [Web IDE]
│   └── 📁 public/
│       ├── 📄 index.html
│       ├── 📄 styles.css
│       ├── 📄 script.js
│       └── 📄 runtime.js
│
└── 📁 tests/                      [Test Programs]
    └── 📁 resources/input/
        ├── 📄 test_simple.code
        ├── 📄 test_conditional.code
        ├── 📄 test_loop.code
        └── 📄 test_error_undeclared.code
```

---

**This comprehensive set of diagrams shows how every component of the 59LANG compiler works together!**
