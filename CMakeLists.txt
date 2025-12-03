cmake_minimum_required(VERSION 3.10)
project(MiniCompilerCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add jsoncpp as subdirectory
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(external/jsoncpp)

# Source files
set(SOURCES
    src/main.cpp
    src/scanner.cpp
    src/token.cpp
    src/parser.cpp
    src/ast_node.cpp
    src/symbol_table.cpp
)

# Create executable
add_executable(compiler ${SOURCES})

# Link libraries
target_link_libraries(compiler jsoncpp_lib)
target_include_directories(compiler PRIVATE ${PROJECT_SOURCE_DIR}/external/jsoncpp/include)

# Optional: Add test executable
enable_testing()
