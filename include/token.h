#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FUNC, VAR, CONST, RETURN, IF, ELSE, WHILE, FOR, DO, BREAK, CONTINUE,
    SWITCH, CASE, DEFAULT, INPUT, OUTPUT, INT, FLOAT, BOOL, STRING, TRUE, FALSE,
    AND, OR, NOT, MAIN, END,
    
    // New keywords (nexus, shard, core, flux, sig, glyph, probe, fallback, pulse, cycle, listen, broadcast, join, either, void)
    NEXUS, SHARD, CORE, FLUX, SIG, GLYPH, PROBE, FALLBACK, PULSE, CYCLE, LISTEN, BROADCAST, JOIN, EITHER, VOID_NOT,
    
    // Identifiers and literals
    IDENTIFIER, NUMBER, FLOAT_NUMBER, STRING_LITERAL,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER,
    ASSIGN, EQUAL, NOT_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT, BITWISE_AND, BITWISE_OR, BITWISE_XOR,
    LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT,
    
    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT, COLON, ARROW, QUESTION,
    
    // Special
    END_OF_FILE, NEWLINE, ERROR_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::ERROR_TOKEN, const std::string& v = "", 
          int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    std::string typeToString() const;
};

#endif // TOKEN_H
