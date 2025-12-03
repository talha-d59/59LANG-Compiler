#include "../include/scanner.h"
#include <cctype>
#include <algorithm>

Scanner::Scanner(const std::string& src)
    : source(src), position(0), line(1), column(1) {
    initializeKeywords();
}

void Scanner::initializeKeywords() {
    keywords = {
        {"func", TokenType::FUNC},
        {"var", TokenType::VAR},
        {"const", TokenType::CONST},
        {"return", TokenType::RETURN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"do", TokenType::DO},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE},
        {"default", TokenType::DEFAULT},
        {"input", TokenType::INPUT},
        {"output", TokenType::OUTPUT},
        {"int", TokenType::INT},
        {"float", TokenType::FLOAT},
        {"bool", TokenType::BOOL},
        {"string", TokenType::STRING},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"main", TokenType::MAIN},
        {"end", TokenType::END},
        // New syntax keywords
        {"nexus", TokenType::NEXUS},
        {"shard", TokenType::SHARD},
        {"core", TokenType::CORE},
        {"flux", TokenType::FLUX},
        {"sig", TokenType::SIG},
        {"glyph", TokenType::GLYPH},
        {"probe", TokenType::PROBE},
        {"fallback", TokenType::FALLBACK},
        {"pulse", TokenType::PULSE},
        {"cycle", TokenType::CYCLE},
        {"listen", TokenType::LISTEN},
        {"broadcast", TokenType::BROADCAST},
        {"join", TokenType::JOIN},
        {"either", TokenType::EITHER},
        {"void", TokenType::VOID_NOT}
    };
}

char Scanner::currentChar() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Scanner::peekChar(int offset) {
    if (position + offset >= source.length()) return '\0';
    return source[position + offset];
}

void Scanner::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Scanner::skipWhitespace() {
    while (std::isspace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

void Scanner::skipComment() {
    if (currentChar() == '%') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
    }
}

Token Scanner::scanString() {
    int startCol = column;
    advance(); // skip opening quote
    std::string value;
    
    while (currentChar() != '"' && currentChar() != '\0') {
        if (currentChar() == '\\') {
            advance();
            switch (currentChar()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += currentChar();
            }
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (currentChar() == '"') {
        advance(); // skip closing quote
    } else {
        errors.push_back(std::make_shared<Error>(
            "Unterminated string literal", line, startCol, ErrorType::SCANNER
        ));
    }
    
    return Token(TokenType::STRING_LITERAL, value, line, startCol);
}

Token Scanner::scanNumber() {
    int startCol = column;
    std::string value;
    
    while (std::isdigit(currentChar())) {
        value += currentChar();
        advance();
    }
    
    if (currentChar() == '.' && std::isdigit(peekChar())) {
        value += '.';
        advance();
        while (std::isdigit(currentChar())) {
            value += currentChar();
            advance();
        }
        return Token(TokenType::FLOAT_NUMBER, value, line, startCol);
    }
    
    return Token(TokenType::NUMBER, value, line, startCol);
}

Token Scanner::scanIdentifierOrKeyword() {
    int startCol = column;
    std::string value;
    
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        value += currentChar();
        advance();
    }
    
    // Convert to lowercase for keyword matching
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    
    auto it = keywords.find(lowerValue);
    if (it != keywords.end()) {
        return Token(it->second, value, line, startCol);
    }
    
    return Token(TokenType::IDENTIFIER, value, line, startCol);
}

Token Scanner::scanOperatorOrPunctuation() {
    int startCol = column;
    char current = currentChar();
    
    switch (current) {
        case '+':
            advance();
            if (currentChar() == '+') {
                advance();
                return Token(TokenType::INCREMENT, "++", line, startCol);
            }
            return Token(TokenType::PLUS, "+", line, startCol);
        case '-':
            advance();
            if (currentChar() == '-') {
                advance();
                return Token(TokenType::DECREMENT, "--", line, startCol);
            }
            if (currentChar() == '>') {
                advance();
                return Token(TokenType::ARROW, "->", line, startCol);
            }
            return Token(TokenType::MINUS, "-", line, startCol);
        case '*':
            advance();
            if (currentChar() == '*') {
                advance();
                return Token(TokenType::POWER, "**", line, startCol);
            }
            return Token(TokenType::MULTIPLY, "*", line, startCol);
        case '/':
            advance();
            return Token(TokenType::DIVIDE, "/", line, startCol);
        case '%':
            advance();
            return Token(TokenType::MODULO, "%", line, startCol);
        case '=':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::EQUAL, "==", line, startCol);
            }
            return Token(TokenType::ASSIGN, "=", line, startCol);
        case '!':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", line, startCol);
            }
            return Token(TokenType::LOGICAL_NOT, "!", line, startCol);
        case '<':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", line, startCol);
            }
            if (currentChar() == '<') {
                advance();
                return Token(TokenType::LEFT_SHIFT, "<<", line, startCol);
            }
            return Token(TokenType::LESS, "<", line, startCol);
        case '>':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", line, startCol);
            }
            if (currentChar() == '>') {
                advance();
                return Token(TokenType::RIGHT_SHIFT, ">>", line, startCol);
            }
            return Token(TokenType::GREATER, ">", line, startCol);
        case '&':
            advance();
            if (currentChar() == '&') {
                advance();
                return Token(TokenType::LOGICAL_AND, "&&", line, startCol);
            }
            return Token(TokenType::BITWISE_AND, "&", line, startCol);
        case '|':
            advance();
            if (currentChar() == '|') {
                advance();
                return Token(TokenType::LOGICAL_OR, "||", line, startCol);
            }
            return Token(TokenType::BITWISE_OR, "|", line, startCol);
        case '^':
            advance();
            return Token(TokenType::BITWISE_XOR, "^", line, startCol);
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", line, startCol);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", line, startCol);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", line, startCol);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", line, startCol);
        case '[':
            advance();
            return Token(TokenType::LBRACKET, "[", line, startCol);
        case ']':
            advance();
            return Token(TokenType::RBRACKET, "]", line, startCol);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", line, startCol);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", line, startCol);
        case '.':
            advance();
            return Token(TokenType::DOT, ".", line, startCol);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", line, startCol);
        case '?':
            advance();
            return Token(TokenType::QUESTION, "?", line, startCol);
        default:
            advance();
            errors.push_back(std::make_shared<Error>(
                std::string("Illegal character '") + current + "'",
                line, startCol, ErrorType::SCANNER
            ));
            return Token(TokenType::ERROR_TOKEN, std::string(1, current), line, startCol);
    }
}

Token Scanner::nextToken() {
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) break;
        
        if (currentChar() == '%') {
            skipComment();
            continue;
        }
        
        if (currentChar() == '\n') {
            int col = column;
            advance();
            return Token(TokenType::NEWLINE, "\n", line - 1, col);
        }
        
        if (currentChar() == '"') {
            return scanString();
        }
        
        if (std::isdigit(currentChar())) {
            return scanNumber();
        }
        
        if (std::isalpha(currentChar()) || currentChar() == '_') {
            return scanIdentifierOrKeyword();
        }
        
        return scanOperatorOrPunctuation();
    }
    
    return Token(TokenType::END_OF_FILE, "", line, column);
}

void Scanner::reset() {
    position = 0;
    line = 1;
    column = 1;
    errors.clear();
}

std::string Scanner::tokenTypeToString(TokenType type) {
    Token t(type);
    return t.typeToString();
}
