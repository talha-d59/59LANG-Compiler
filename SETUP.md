# 59LANG Setup & Installation Guide

Complete installation and configuration guide for the 59LANG compiler system with CLI and web IDE.

## Prerequisites

### Required Software
- **C++17 compatible compiler**: g++, clang, or MSVC
- **CMake**: 3.10 or higher
- **Python**: 3.7 or higher
- **jsoncpp**: C++ JSON library

### Windows Installation
```powershell
# Visual Studio C++ build tools (if not installed)
# Download from: https://visualstudio.microsoft.com/downloads/

# CMake
# Download from: https://cmake.org/download/

# Python
# Download from: https://www.python.org/

# jsoncpp (via vcpkg recommended)
vcpkg install jsoncpp:x64-windows
```

### Ubuntu/Debian Installation
```bash
sudo apt-get update
sudo apt-get install build-essential cmake python3 python3-pip
sudo apt-get install libjsoncpp-dev
```

### macOS Installation
```bash
brew install cmake python3 jsoncpp
```

## Build Instructions

### Step 1: Clone and Navigate
```bash
cd mini-compiler-cpp
```

### Step 2: Build C++ Compiler
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
```

If cmake can't find jsoncpp:

**Windows (with vcpkg)**:
```powershell
vcpkg install jsoncpp:x64-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
```

**Linux/Mac**:
```bash
sudo apt-get install libjsoncpp-dev  # Ubuntu/Debian
# or
brew install jsoncpp  # macOS
```

### Step 3: Test CLI Compiler
```bash
# Windows
.\build\Release\compiler.exe tests/resources/input/test_simple.code

# Linux/Mac
./build/compiler tests/resources/input/test_simple.code

# With JSON output
./build/compiler tests/resources/input/test_simple.code --json
```

### Step 4: Setup Flask Backend
```bash
cd backend
pip install -r requirements.txt
python app.py
```

The backend will start on `http://localhost:5000`

### Step 5: Run Web Frontend
```bash
cd frontend/public
python -m http.server 3000
```

### Step 6: Access the IDE
Open your browser:
- **Web IDE**: `http://localhost:3000`
- **Backend API**: `http://localhost:5000/api/health`

## Directory Structure After Build
```
mini-compiler-cpp/
├── build/
│   ├── Release/
│   │   └── compiler.exe      (Windows executable)
│   ├── compiler              (Linux/Mac executable)
│   ├── CMakeFiles/
│   ├── cmake_install.cmake
│   ├── CMakeCache.txt
│   └── Makefile (Linux/Mac)
├── frontend/
│   └── public/
│       ├── index.html
│       ├── styles.css
│       ├── script.js
│       └── runtime.js
├── backend/
│   ├── app.py
│   └── requirements.txt
├── src/
├── include/
└── tests/
    └── resources/input/
```

## Quick Start Script

### Linux/Mac
```bash
#!/bin/bash
# Build
mkdir -p build && cd build && cmake .. && cmake --build .
cd ..

# Backend (in new terminal)
cd backend && pip install -r requirements.txt && python app.py &

# Frontend (in another terminal)
cd frontend/public && python -m http.server 3000 &

echo "Open http://localhost:3000 in your browser"
```

### Windows (PowerShell)
```powershell
# Build
New-Item -ItemType Directory -Path build -Force
Set-Location build
cmake ..
cmake --build . --config Release
Set-Location ..

# Backend (in new terminal)
Set-Location backend
pip install -r requirements.txt
python app.py

# Frontend (in another new terminal)
Set-Location frontend\public
python -m http.server 3000
```

## Usage

### CLI Usage
```bash
# Compile with error display
./build/compiler program.code

# Compile with JSON output
./build/compiler program.code --json

# Example
./build/compiler tests/resources/input/test_simple.code --json
```

### Web IDE Usage
1. Navigate to `http://localhost:3000`
2. Write or paste 59LANG code
3. Click "Compile" or press Ctrl+Enter
4. Click "Run" to execute with runtime console
5. View errors in the Errors tab
6. View symbols in the Symbol Table tab
7. Use example dropdown for sample programs

## Example 59LANG Program
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

## Troubleshooting

### CMake can't find jsoncpp
```bash
# Install jsoncpp development files
# Ubuntu/Debian:
sudo apt-get install libjsoncpp-dev

# macOS:
brew install jsoncpp

# Windows with vcpkg:
vcpkg install jsoncpp:x64-windows
```

### Compiler executable not found
Make sure you built successfully:
```bash
cd build
cmake ..
cmake --build . --config Release
```

### Backend connection errors
- Ensure Flask is running on port 5000
- Check firewall settings
- Try: `python app.py` for debug output

### CORS errors in browser
- Flask backend has CORS enabled
- Check browser console for specific errors
- Verify API_BASE in `frontend/public/script.js`

### Port already in use
- **Backend (5000)**: Change port in `app.py`
- **Frontend (3000)**: Use different port: `python -m http.server 8000`

## Next Steps

- Read **LANGUAGE_REFERENCE.md** for complete syntax guide
- See **ARCHITECTURE.md** for system architecture
- Explore `tests/resources/input/` for more examples
- Modify compiler by editing files in `src/` and `include/`

---

**For language syntax, see LANGUAGE_REFERENCE.md. For architecture, see ARCHITECTURE.md.**
