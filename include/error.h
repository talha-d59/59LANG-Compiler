#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>

enum class ErrorType {
    SCANNER,    // Lexical error
    PARSER,     // Syntax error
    SEMANTIC    // Semantic error
};

struct Error {
    std::string message;
    int line;
    int column;
    ErrorType type;
    std::string context;

    Error(const std::string& msg, int l, int c, ErrorType t, const std::string& ctx = "")
        : message(msg), line(l), column(c), type(t), context(ctx) {}

    std::string typeToString() const {
        switch (type) {
            case ErrorType::SCANNER: return "SCANNER";
            case ErrorType::PARSER: return "PARSER";
            case ErrorType::SEMANTIC: return "SEMANTIC";
            default: return "UNKNOWN";
        }
    }

    std::string toString() const {
        return "ERROR(" + typeToString() + "): " + message + 
               " at line " + std::to_string(line) + ", column " + std::to_string(column);
    }
};

#endif // ERROR_H
