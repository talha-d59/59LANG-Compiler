# 59LANG Compiler

A complete compiler implementation for the **59LANG** programming language with C++ compiler backend, Flask API server, and modern web IDE.

## Features

- **Full Compiler Pipeline**: Lexical analysis, syntax parsing, semantic validation
- **Interactive Web IDE**: Real-time compilation feedback with syntax highlighting
- **REST API**: Flask backend for web integration
- **CLI Tool**: Command-line compiler with JSON output
- **Cross-Platform**: Windows, Linux, macOS support

## Quick Start

### Prerequisites
- C++17 compiler (g++, clang, or MSVC)
- CMake 3.10+
- Python 3.7+
- jsoncpp library

### Installation

```bash
# Clone and build
cd mini-compiler-cpp
mkdir build && cd build
cmake ..
cmake --build . --config Release
cd ..

# Install Python dependencies
cd backend
pip install -r requirements.txt
cd ..
```

### Run

```bash
# Terminal 1: Start backend
cd backend
python app.py

# Terminal 2: Start frontend
cd frontend/public
python -m http.server 3000

# Open http://localhost:3000
```

## 59LANG Syntax

### Basic Program
```
nexus {
    shard core x = 5;
    broadcast x;
}
```

### Keywords
- **nexus** - Program entry point
- **shard** - Variable declaration
- **core/flux/sig/glyph** - Types (int/float/bool/string)
- **listen/broadcast** - Input/output
- **probe/fallback** - If/else
- **pulse/cycle** - While/for loops

### Example Programs

**Simple I/O:**
```
nexus {
    shard core x, y;
    listen x;
    listen y;
    shard core sum = x + y;
    broadcast sum;
}
```

**Conditional:**
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

**Loop:**
```
nexus {
    shard core i = 1;
    pulse (i <= 5) {
        broadcast i;
        i = i + 1;
    }
}
```

**Factorial:**
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

## Project Structure

```
mini-compiler-cpp/
├── include/              # C++ headers
│   ├── scanner.h        # Lexical analyzer
│   ├── parser.h         # Syntax parser
│   ├── ast_node.h       # AST definitions
│   ├── symbol_table.h   # Symbol management
│   ├── token.h          # Token types
│   └── error.h          # Error handling
├── src/                 # C++ implementation
│   ├── scanner.cpp
│   ├── parser.cpp
│   ├── ast_node.cpp
│   ├── symbol_table.cpp
│   ├── token.cpp
│   └── main.cpp         # CLI entry point
├── backend/             # Flask API
│   ├── app.py
│   └── requirements.txt
├── frontend/            # Web IDE
│   └── public/
│       ├── index.html
│       ├── styles.css
│       ├── script.js
│       └── runtime.js
├── tests/               # Test programs
│   └── resources/input/
├── CMakeLists.txt       # Build config
└── README.md            # This file
```

## Architecture

### Compilation Pipeline

```
Source Code → [Scanner] → Tokens → [Parser] → AST → [Semantic] → Errors/Symbols
```

### System Architecture

```
┌─────────────────┐
│   Web Browser   │ (Frontend - HTML/CSS/JS)
└────────┬────────┘
         │ HTTP POST /api/compile
         ↓
┌─────────────────┐
│  Flask Server   │ (Backend - Python)
└────────┬────────┘
         │ Execute subprocess
         ↓
┌─────────────────┐
│  C++ Compiler   │ (Compiler - C++)
└─────────────────┘
```

### Error Detection

Three error types are reported:

- **SCANNER** - Illegal characters, malformed literals
- **PARSER** - Syntax errors, missing semicolons, unmatched braces
- **SEMANTIC** - Undeclared variables, duplicate declarations

## Usage

### Web IDE

1. Open `http://localhost:3000`
2. Write 59LANG code
3. Click "Compile" or press Ctrl+Enter
4. Click "Run" to execute with runtime console
5. View errors, symbols, and output in tabs

### Command Line

```bash
# Compile and show errors
./build/compiler program.code

# JSON output
./build/compiler program.code --json
```

### API

```bash
# Compile code
curl -X POST http://localhost:5000/api/compile \
  -H "Content-Type: application/json" \
  -d '{"code": "nexus { shard core x = 5; broadcast x; }"}'

# Get examples
curl http://localhost:5000/api/examples

# Health check
curl http://localhost:5000/api/health
```

## Development

### Building from Source

See [SETUP.md](SETUP.md) for detailed instructions.

**Quick build:**
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Adding Features

**New keyword:**
1. Add to `TokenType` enum in `include/token.h`
2. Add to keywords map in `include/scanner.h`
3. Add grammar rule in `src/parser.cpp`

**New statement:**
1. Add `parse*Statement()` method in `include/parser.h`
2. Implement in `src/parser.cpp`
3. Call from `parseStatement()`

**Frontend changes:**
- Edit `frontend/public/index.html` for UI
- Edit `frontend/public/styles.css` for styling
- Edit `frontend/public/script.js` for logic
- Edit `frontend/public/runtime.js` for execution

### Testing

```bash
# Run compiler on test files
./build/compiler tests/resources/input/test_simple.code --json
./build/compiler tests/resources/input/test_conditional.code --json
./build/compiler tests/resources/input/test_loop.code --json
```

## Documentation

- **README.md** (this file) - Overview and quick start
- **SETUP.md** - Detailed installation instructions
- **LANGUAGE_REFERENCE.md** - Complete syntax guide
- **ARCHITECTURE.md** - Visual diagrams and data flow

## Technology Stack

- **Compiler**: C++17, CMake, jsoncpp
- **Backend**: Python, Flask, flask-cors
- **Frontend**: HTML5, CSS3, JavaScript
- **Build**: CMake, Make/MSBuild

## License

MIT License - Feel free to use for educational purposes.

## Author

Educational compiler project demonstrating modern compiler construction.
