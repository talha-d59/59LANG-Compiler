#include "../include/token.h"
#include <map>

std::string Token::typeToString() const {
    static const std::map<TokenType, std::string> typeMap = {
        // Keywords
        {TokenType::FUNC, "FUNC"}, {TokenType::VAR, "VAR"}, {TokenType::CONST, "CONST"},
        {TokenType::RETURN, "RETURN"}, {TokenType::IF, "IF"}, {TokenType::ELSE, "ELSE"},
        {TokenType::WHILE, "WHILE"}, {TokenType::FOR, "FOR"}, {TokenType::DO, "DO"},
        {TokenType::BREAK, "BREAK"}, {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::SWITCH, "SWITCH"}, {TokenType::CASE, "CASE"}, {TokenType::DEFAULT, "DEFAULT"},
        {TokenType::INPUT, "INPUT"}, {TokenType::OUTPUT, "OUTPUT"},
        {TokenType::INT, "INT"}, {TokenType::FLOAT, "FLOAT"}, {TokenType::BOOL, "BOOL"},
        {TokenType::STRING, "STRING"}, {TokenType::TRUE, "TRUE"}, {TokenType::FALSE, "FALSE"},
        {TokenType::AND, "AND"}, {TokenType::OR, "OR"}, {TokenType::NOT, "NOT"},
        {TokenType::MAIN, "MAIN"}, {TokenType::END, "END"},
        
        // Identifiers and literals
        {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::NUMBER, "NUMBER"},
        {TokenType::FLOAT_NUMBER, "FLOAT_NUMBER"}, {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        
        // Operators
        {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"}, {TokenType::MULTIPLY, "MULTIPLY"},
        {TokenType::DIVIDE, "DIVIDE"}, {TokenType::MODULO, "MODULO"}, {TokenType::POWER, "POWER"},
        {TokenType::ASSIGN, "ASSIGN"}, {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS, "LESS"}, {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"}, {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LOGICAL_AND, "LOGICAL_AND"}, {TokenType::LOGICAL_OR, "LOGICAL_OR"},
        {TokenType::LOGICAL_NOT, "LOGICAL_NOT"}, {TokenType::BITWISE_AND, "BITWISE_AND"},
        {TokenType::BITWISE_OR, "BITWISE_OR"}, {TokenType::BITWISE_XOR, "BITWISE_XOR"},
        {TokenType::LEFT_SHIFT, "LEFT_SHIFT"}, {TokenType::RIGHT_SHIFT, "RIGHT_SHIFT"},
        {TokenType::INCREMENT, "INCREMENT"}, {TokenType::DECREMENT, "DECREMENT"},
        
        // Punctuation
        {TokenType::LPAREN, "LPAREN"}, {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACE, "LBRACE"}, {TokenType::RBRACE, "RBRACE"},
        {TokenType::LBRACKET, "LBRACKET"}, {TokenType::RBRACKET, "RBRACKET"},
        {TokenType::SEMICOLON, "SEMICOLON"}, {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"}, {TokenType::COLON, "COLON"},
        {TokenType::ARROW, "ARROW"}, {TokenType::QUESTION, "QUESTION"},
        
        // Special
        {TokenType::END_OF_FILE, "END_OF_FILE"}, {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::ERROR_TOKEN, "ERROR_TOKEN"}
    };
    
    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "UNKNOWN";
}
