#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symbol_table.h"
#include "ast_node.h"
#include "error.h"
#include <vector>
#include <memory>

class Parser {
private:
    Scanner scanner;
    std::vector<Token> tokens;
    size_t current;
    std::vector<std::shared_ptr<Error>> errors;
    SymbolTable symbolTable;
    
    // Utility methods
    Token peek() const;
    Token peekAhead(int distance) const;
    Token advance();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    void consume(TokenType type, const std::string& message);
    void error(const std::string& message, int line, int column, ErrorType type);
    
    // Parsing methods (recursive descent)
    ASTNodePtr parseProgram();
    ASTNodeList parseDeclarations();
    ASTNodePtr parseDeclaration();
    ASTNodeList parseStatements();
    ASTNodePtr parseStatement();
    ASTNodePtr parseAssignment();
    ASTNodePtr parseIfStatement();
    ASTNodePtr parseWhileLoop();
    ASTNodePtr parseForLoop();
    ASTNodePtr parseReturnStatement();
    ASTNodePtr parseExpression();
    ASTNodePtr parseLogicalOr();
    ASTNodePtr parseLogicalAnd();
    ASTNodePtr parseEquality();
    ASTNodePtr parseComparison();
    ASTNodePtr parseAddition();
    ASTNodePtr parseMultiplication();
    ASTNodePtr parseUnary();
    ASTNodePtr parsePrimary();
    ASTNodePtr parseFunctionCall();
    
    // Semantic analysis
    void validateIdentifier(const std::string& name, int line, int column);
    void declareIdentifier(const std::string& name, const std::string& type, int line, int column);
    
public:
    Parser(const std::string& source);
    ASTNodePtr parse();
    std::vector<std::shared_ptr<Error>> getErrors() const { return errors; }
    SymbolTable getSymbolTable() const { return symbolTable; }
    bool hasErrors() const { return !errors.empty(); }
};

#endif // PARSER_H
