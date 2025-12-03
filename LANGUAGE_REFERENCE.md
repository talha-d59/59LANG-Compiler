# 59LANG Language Reference

Complete syntax documentation for the 59LANG programming language.

## Quick Start

### Minimal Program
```
nexus {
    broadcast "Hello, World!";
}
```

### With Variables
```
nexus {
    shard core x = 5;
    broadcast x;
}
```

## Program Structure

### Entry Point
```
nexus {
    [declarations]
    [statements]
}
```

The `nexus` keyword defines the main program block. All code executes within this block.

### Variable Declaration
```
shard core x;              % Single integer
shard core x, y, z;        % Multiple integers
shard flux pi = 3.14;      % Float with initialization
shard sig flag;            % Boolean
shard glyph msg;           % String
```

## Data Types

| Keyword | Type | Example | Description |
|---------|------|---------|-------------|
| `core` | Integer | `42`, `-5`, `0` | Whole numbers |
| `flux` | Float | `3.14`, `-2.5` | Decimal numbers |
| `sig` | Boolean | `true`, `false` | Truth values |
| `glyph` | String | `"hello"` | Text strings |

## Operators

### Arithmetic
| Operator | Meaning | Example | Description |
|----------|---------|---------|-------------|
| `+` | Addition | `a + b` | Sum of a and b |
| `-` | Subtraction | `a - b` | Difference of a and b |
| `*` | Multiplication | `a * b` | Product of a and b |
| `/` | Division | `a / b` | Quotient of a and b |
| `%` | Modulo | `a % b` | Remainder of a / b |
| `**` | Power | `a ** b` | a raised to power b |

### Comparison
| Operator | Meaning | Example | Description |
|----------|---------|---------|-------------|
| `==` | Equal | `a == b` | True if equal |
| `!=` | Not equal | `a != b` | True if not equal |
| `<` | Less than | `a < b` | True if a less than b |
| `<=` | Less or equal | `a <= b` | True if a ≤ b |
| `>` | Greater than | `a > b` | True if a greater than b |
| `>=` | Greater or equal | `a >= b` | True if a ≥ b |

### Logical
| Operator | Keyword | Example | Description |
|----------|---------|---------|-------------|
| `join` | AND | `a join b` | True if both true |
| `either` | OR | `a either b` | True if at least one true |
| `void` | NOT | `void a` | True if a is false |

Alternative syntax: `&&`, `||`, `!` also supported.

### Bitwise
| Operator | Meaning | Example | Description |
|----------|---------|---------|-------------|
| `&` | AND | `a & b` | Bitwise AND |
| `\|` | OR | `a \| b` | Bitwise OR |
| `^` | XOR | `a ^ b` | Bitwise XOR |
| `<<` | Left shift | `a << b` | Shift a left by b bits |
| `>>` | Right shift | `a >> b` | Shift a right by b bits |

### Assignment & Increment
| Operator | Meaning | Example | Description |
|----------|---------|---------|-------------|
| `=` | Assignment | `x = 5` | Assign value to variable |
| `++` | Increment | `x++` | Increase by 1 |
| `--` | Decrement | `x--` | Decrease by 1 |

## Statements

### Assignment
```
x = 10;
y = x + 5;
flag = true;
```

### Input/Output
```
listen x;              % Read from input
broadcast x;           % Print variable
broadcast "text";      % Print string
broadcast 42;          % Print literal
```

### Conditional Statements
```
probe (condition) {
    % True branch
}

probe (condition) {
    % True branch
} fallback {
    % False branch
}
```

The `probe` keyword checks a condition, and `fallback` executes when the condition is false.

### While Loop
```
pulse (condition) {
    % Loop body
}
```

The `pulse` keyword creates a loop that continues while the condition is true.

### For Loop
```
cycle (init; condition; increment) {
    % Loop body
}
```

The `cycle` keyword creates a for loop with initialization, condition, and increment.

### Break & Continue
```
pulse (true) {
    probe (x > 10) {
        break;         % Exit loop
    }
    probe (x == 5) {
        continue;      % Skip iteration
    }
    x = x + 1;
}
```

## Complete Examples

### Simple I/O
```
nexus {
    shard core x, y;
    listen x;
    listen y;
    shard core sum = x + y;
    broadcast sum;
}
```

### Arithmetic Operations
```
nexus {
    shard core a = 10;
    shard core b = 3;
    
    broadcast a + b;      % 13
    broadcast a - b;      % 7
    broadcast a * b;      % 30
    broadcast a / b;      % 3
    broadcast a % b;      % 1
    broadcast a ** b;     % 1000
}
```

### Conditional Logic
```
nexus {
    shard core age;
    listen age;
    
    probe (age >= 18) {
        broadcast "Adult";
    } fallback {
        broadcast "Minor";
    }
}
```

### While Loop (Count to 10)
```
nexus {
    shard core i = 1;
    pulse (i <= 10) {
        broadcast i;
        i = i + 1;
    }
}
```

### Factorial Calculator
```
nexus {
    shard core n;
    listen n;
    
    shard core factorial = 1;
    shard core i = 1;
    
    pulse (i <= n) {
        factorial = factorial * i;
        i = i + 1;
    }
    
    broadcast factorial;
}
```

### Even/Odd Detection
```
nexus {
    shard core num;
    listen num;
    
    probe (num % 2 == 0) {
        broadcast "Even";
    } fallback {
        broadcast "Odd";
    }
}
```

### String Operations
```
nexus {
    shard glyph name;
    listen name;
    broadcast "Hello, ";
    broadcast name;
}
```

### Boolean Logic
```
nexus {
    shard core x = 5;
    shard core y = 10;
    
    probe (x > 0 join y > 0) {
        broadcast "Both positive";
    }
    
    probe (x < 0 either y < 0) {
        broadcast "At least one negative";
    } fallback {
        broadcast "Both non-negative";
    }
}
```

## Comments
```
% This is a comment
shard core x;  % End-of-line comment
```

## Error Types

### SCANNER Errors
Lexical analysis errors occur when invalid characters or malformed tokens are detected.

```
nexus {
    shard core x@;  % Illegal character '@'
}
```

### PARSER Errors
Syntax errors occur when code doesn't follow grammar rules.

```
nexus {
    shard core x   % Missing semicolon
}
```

### SEMANTIC Errors
Semantic errors occur when code is syntactically correct but has logical issues.

```
nexus {
    broadcast x;   % Variable 'x' not declared
}
```

## Keywords Reference

| Category | Keywords |
|----------|----------|
| **Program** | `nexus` |
| **Variables** | `shard` |
| **Types** | `core`, `flux`, `sig`, `glyph` |
| **Control** | `probe`, `fallback`, `pulse`, `cycle` |
| **I/O** | `listen`, `broadcast` |
| **Logic** | `join`, `either`, `void` |
| **Values** | `true`, `false` |
| **Loop Control** | `break`, `continue` |

## Operator Precedence (High to Low)

1. `()` (Grouping)
2. `!` `void` `-` (Unary operators)
3. `**` (Exponentiation)
4. `*` `/` `%` (Multiplication, division, modulo)
5. `+` `-` (Addition, subtraction)
6. `<<` `>>` (Bitwise shifts)
7. `<` `<=` `>` `>=` (Comparison)
8. `==` `!=` (Equality)
9. `&` (Bitwise AND)
10. `^` (Bitwise XOR)
11. `|` (Bitwise OR)
12. `&&` `join` (Logical AND)
13. `||` `either` (Logical OR)
14. `=` (Assignment)

## Escape Sequences (in strings)
| Sequence | Meaning |
|----------|---------|
| `\"` | Double quote |
| `\\` | Backslash |
| `\n` | Newline |
| `\t` | Tab |

## Best Practices

1. **Always declare variables before use**
   ```
   shard core x;  % Declare first
   x = 5;         % Then use
   ```

2. **Use meaningful variable names**
   ```
   shard core age;          % Good
   shard core a;            % Less clear
   ```

3. **Test conditionals carefully**
   ```
   probe (x == 5) {         % Note: == for comparison
       x = 10;              % Note: = for assignment
   }
   ```

4. **Initialize loop counters**
   ```
   shard core i = 0;        % Always start at defined value
   pulse (i < 10) {
       % ...
       i = i + 1;
   }
   ```

5. **Use loops instead of repetitive code**
   ```
   % Bad:
   broadcast 1; broadcast 2; broadcast 3;
   
   % Good:
   shard core i = 1;
   pulse (i <= 3) {
       broadcast i;
       i = i + 1;
   }
   ```

## Common Mistakes

| Mistake | Error Type | Fix |
|---------|------------|-----|
| `shard core x = y;` where y not declared | SEMANTIC | Declare y first |
| `x = 5` without `;` | PARSER | Add semicolon |
| `listen x;` for undeclared x | SEMANTIC | Declare x first |
| `probe x > 5` (missing parens) | PARSER | Use `probe (x > 5)` |
| Using `:=` instead of `=` | PARSER | Use `=` for assignment |
| Forgetting `nexus { }` | PARSER | Must have nexus block |

## Syntax Comparison

For developers familiar with traditional languages, here's how 59LANG maps:

| Traditional | 59LANG | Purpose |
|-------------|--------|---------|
| `main` | `nexus` | Program entry point |
| `var int` | `shard core` | Integer declaration |
| `var float` | `shard flux` | Float declaration |
| `var bool` | `shard sig` | Boolean declaration |
| `var string` | `shard glyph` | String declaration |
| `if` | `probe` | Conditional |
| `else` | `fallback` | Alternative branch |
| `while` | `pulse` | While loop |
| `for` | `cycle` | For loop |
| `input` | `listen` | Read input |
| `output` | `broadcast` | Write output |
| `and` | `join` | Logical AND |
| `or` | `either` | Logical OR |
| `not` | `void` | Logical NOT |

---

**For setup instructions, see SETUP.md. For architectural details, see ARCHITECTURE.md.**
