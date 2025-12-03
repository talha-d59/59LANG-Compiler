#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "error.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class Scanner {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    std::vector<std::shared_ptr<Error>> errors;
    std::unordered_map<std::string, TokenType> keywords;
    
    void initializeKeywords();
    char currentChar();
    char peekChar(int offset = 1);
    void advance();
    void skipWhitespace();
    void skipComment();
    Token scanString();
    Token scanNumber();
    Token scanIdentifierOrKeyword();
    Token scanOperatorOrPunctuation();
    
public:
    Scanner(const std::string& src);
    Token nextToken();
    std::vector<std::shared_ptr<Error>> getErrors() const { return errors; }
    void reset();
    
    static std::string tokenTypeToString(TokenType type);
};

#endif // SCANNER_H
