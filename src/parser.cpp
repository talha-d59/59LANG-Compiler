#include "../include/parser.h"
#include <algorithm>
#include <iostream>

Parser::Parser(const std::string& source)
    : scanner(source), current(0) {}

Token Parser::peek() const {
    if (current < tokens.size()) {
        return tokens[current];
    }
    return Token(TokenType::END_OF_FILE, "", 0, 0);
}

Token Parser::peekAhead(int distance) const {
    if (current + distance < tokens.size()) {
        return tokens[current + distance];
    }
    return Token(TokenType::END_OF_FILE, "", 0, 0);
}

Token Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    return Token(TokenType::END_OF_FILE, "", 0, 0);
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (!check(type)) {
        error(message, peek().line, peek().column, ErrorType::PARSER);
    }
    advance();
}

void Parser::error(const std::string& message, int line, int column, ErrorType type) {
    errors.push_back(std::make_shared<Error>(message, line, column, type));
}

void Parser::declareIdentifier(const std::string& name, const std::string& type, int line, int column) {
    try {
        symbolTable.addSymbol(name, type, line, column);
    } catch (const std::runtime_error& e) {
        error(e.what(), line, column, ErrorType::SEMANTIC);
    }
}

void Parser::validateIdentifier(const std::string& name, int line, int column) {
    if (!symbolTable.exists(name)) {
        error("Symbol '" + name + "' not declared", line, column, ErrorType::SEMANTIC);
    }
}

// Parse methods (recursive descent)
ASTNodePtr Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    
    if (!match(TokenType::MAIN) && !match(TokenType::NEXUS)) {
        error("Expected 'main' or 'nexus' keyword", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    program->name = "main";
    
    if (!match(TokenType::LBRACE)) {
        error("Expected '{' after 'main'", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    program->declarations = parseDeclarations();
    program->statements = parseStatements();
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' at end of program", peek().line, peek().column, ErrorType::PARSER);
    }
    
    if (!check(TokenType::END_OF_FILE)) {
        error("Unexpected token after program end", peek().line, peek().column, ErrorType::PARSER);
    }
    
    return program;
}

ASTNodeList Parser::parseDeclarations() {
    ASTNodeList declarations;
    
    while (check(TokenType::VAR) || check(TokenType::SHARD)) {
        auto decl = parseDeclaration();
        if (decl) {
            declarations.push_back(decl);
        }
    }
    
    return declarations;
}

ASTNodePtr Parser::parseDeclaration() {
    if (!match(TokenType::VAR) && !match(TokenType::SHARD)) {
        error("Expected 'var' keyword", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    auto declaration = std::make_shared<Declaration>();
    
    // Parse type (accept both old and new keywords)
    if (match(TokenType::INT) || match(TokenType::CORE)) {
        declaration->dataType = "int";
    } else if (match(TokenType::FLOAT) || match(TokenType::FLUX)) {
        declaration->dataType = "float";
    } else if (match(TokenType::BOOL) || match(TokenType::SIG)) {
        declaration->dataType = "bool";
    } else if (match(TokenType::STRING) || match(TokenType::GLYPH)) {
        declaration->dataType = "string";
    } else {
        error("Expected type specifier", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    // Parse identifiers (with optional initialization)
    do {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected identifier", peek().line, peek().column, ErrorType::PARSER);
            return nullptr;
        }
        
        Token idToken = advance();
        declaration->identifiers.push_back(idToken.value);
        declareIdentifier(idToken.value, declaration->dataType, idToken.line, idToken.column);
        
        // Check for initialization
        if (match(TokenType::ASSIGN)) {
            ASTNodePtr initExpr = parseExpression();
            if (!initExpr) {
                error("Expected expression after '='", peek().line, peek().column, ErrorType::PARSER);
                return nullptr;
            }
            declaration->initializers.push_back(initExpr);
        } else {
            declaration->initializers.push_back(nullptr);  // No initialization
        }
        
    } while (match(TokenType::COMMA));
    
    if (!match(TokenType::SEMICOLON)) {
        error("Expected ';' after declaration", peek().line, peek().column, ErrorType::PARSER);
    }
    
    return declaration;
}

ASTNodeList Parser::parseStatements() {
    ASTNodeList statements;
    
    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
    }
    
    return statements;
}

ASTNodePtr Parser::parseStatement() {
    // Allow declarations within statement blocks
    if (check(TokenType::VAR) || check(TokenType::SHARD)) {
        return parseDeclaration();
    } else if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    } else if (match(TokenType::IF) || match(TokenType::PROBE)) {
        return parseIfStatement();
    } else if (match(TokenType::WHILE) || match(TokenType::PULSE)) {
        return parseWhileLoop();
    } else if (match(TokenType::FOR) || match(TokenType::CYCLE)) {
        return parseForLoop();
    } else if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    } else if (match(TokenType::INPUT) || match(TokenType::LISTEN)) {
        if (check(TokenType::IDENTIFIER)) {
            Token id = advance();
            validateIdentifier(id.value, id.line, id.column);
            // Require semicolon after input statement
            consume(TokenType::SEMICOLON, "Expected ';' after input");
            auto identifier = std::make_shared<Identifier>();
            identifier->name = id.value;
            auto funcCall = std::make_shared<FunctionCall>();
            funcCall->functionName = "input";
            funcCall->arguments = std::vector<ASTNodePtr>{identifier};
            return funcCall;
        } else {
            error("Expected identifier after 'input'", peek().line, peek().column, ErrorType::PARSER);
            return nullptr;
        }
    } else if (match(TokenType::OUTPUT) || match(TokenType::BROADCAST)) {
        auto expr = parseExpression();
        // Require semicolon after output/broadcast
        consume(TokenType::SEMICOLON, "Expected ';' after output");
        auto funcCall = std::make_shared<FunctionCall>();
        funcCall->functionName = "output";
        funcCall->arguments = std::vector<ASTNodePtr>{expr};
        return funcCall;
    }
    
    error("Unexpected token in statement", peek().line, peek().column, ErrorType::PARSER);
    advance();
    return nullptr;
}

ASTNodePtr Parser::parseAssignment() {
    Token id = advance();
    validateIdentifier(id.value, id.line, id.column);
    
    if (!match(TokenType::ASSIGN)) {
        error("Expected '=' in assignment", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    auto expr = parseExpression();
    
    if (!match(TokenType::SEMICOLON)) {
        error("Expected ';' after assignment", peek().line, peek().column, ErrorType::PARSER);
    }
    
    auto assignment = std::make_shared<Assignment>();
    assignment->identifier = id.value;
    assignment->expression = expr;
    return assignment;
}

ASTNodePtr Parser::parseIfStatement() {
    auto ifStmt = std::make_shared<IfStatement>();
    
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after 'if'", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    ifStmt->condition = parseExpression();
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after condition", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    if (!match(TokenType::LBRACE)) {
        error("Expected '{' after if condition", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    ifStmt->thenBranch = parseStatements();
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' after if block", peek().line, peek().column, ErrorType::PARSER);
    }
    
    if (match(TokenType::ELSE) || match(TokenType::FALLBACK)) {
        if (!match(TokenType::LBRACE)) {
            error("Expected '{' after 'else'", peek().line, peek().column, ErrorType::PARSER);
            return nullptr;
        }
        ifStmt->elseBranch = parseStatements();
        if (!match(TokenType::RBRACE)) {
            error("Expected '}' after else block", peek().line, peek().column, ErrorType::PARSER);
        }
    }
    
    return ifStmt;
}

ASTNodePtr Parser::parseWhileLoop() {
    auto whileLoop = std::make_shared<WhileLoop>();
    
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after 'while'", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    whileLoop->condition = parseExpression();
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after condition", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    if (!match(TokenType::LBRACE)) {
        error("Expected '{' after while condition", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    whileLoop->body = parseStatements();
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' after while block", peek().line, peek().column, ErrorType::PARSER);
    }
    
    return whileLoop;
}

ASTNodePtr Parser::parseForLoop() {
    auto forLoop = std::make_shared<ForLoop>();
    
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after 'for'", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    forLoop->initialization = parseAssignment();
    forLoop->condition = parseExpression();
    
    if (!match(TokenType::SEMICOLON)) {
        error("Expected ';' after for condition", peek().line, peek().column, ErrorType::PARSER);
    }
    
    forLoop->increment = parseExpression();
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after for clauses", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    if (!match(TokenType::LBRACE)) {
        error("Expected '{' after for", peek().line, peek().column, ErrorType::PARSER);
        return nullptr;
    }
    
    forLoop->body = parseStatements();
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' after for block", peek().line, peek().column, ErrorType::PARSER);
    }
    
    return forLoop;
}

ASTNodePtr Parser::parseReturnStatement() {
    auto retStmt = std::make_shared<ReturnStatement>();
    retStmt->expression = parseExpression();
    
    if (!match(TokenType::SEMICOLON)) {
        error("Expected ';' after return", peek().line, peek().column, ErrorType::PARSER);
    }
    
    return retStmt;
}

ASTNodePtr Parser::parseExpression() {
    return parseLogicalOr();
}

ASTNodePtr Parser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (match({TokenType::LOGICAL_OR, TokenType::OR, TokenType::EITHER})) {
        auto op = std::make_shared<BinaryOp>();
        op->operation = "||";
        op->left = left;
        op->right = parseLogicalAnd();
        left = op;
    }
    
    return left;
}

ASTNodePtr Parser::parseLogicalAnd() {
    auto left = parseEquality();
    
    while (match({TokenType::LOGICAL_AND, TokenType::AND, TokenType::JOIN})) {
        auto op = std::make_shared<BinaryOp>();
        op->operation = "&&";
        op->left = left;
        op->right = parseEquality();
        left = op;
    }
    
    return left;
}

ASTNodePtr Parser::parseEquality() {
    auto left = parseComparison();
    
    while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
        Token op = tokens[current - 1];
        auto opNode = std::make_shared<BinaryOp>();
        opNode->operation = (op.type == TokenType::EQUAL) ? "==" : "!=";
        opNode->left = left;
        opNode->right = parseComparison();
        left = opNode;
    }
    
    return left;
}

ASTNodePtr Parser::parseComparison() {
    auto left = parseAddition();
    
    while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        Token op = tokens[current - 1];
        auto opNode = std::make_shared<BinaryOp>();
        switch (op.type) {
            case TokenType::LESS: opNode->operation = "<"; break;
            case TokenType::LESS_EQUAL: opNode->operation = "<="; break;
            case TokenType::GREATER: opNode->operation = ">"; break;
            case TokenType::GREATER_EQUAL: opNode->operation = ">="; break;
            default: break;
        }
        opNode->left = left;
        opNode->right = parseAddition();
        left = opNode;
    }
    
    return left;
}

ASTNodePtr Parser::parseAddition() {
    auto left = parseMultiplication();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto opNode = std::make_shared<BinaryOp>();
        opNode->operation = (op.type == TokenType::PLUS) ? "+" : "-";
        opNode->left = left;
        opNode->right = parseMultiplication();
        left = opNode;
    }
    
    return left;
}

ASTNodePtr Parser::parseMultiplication() {
    auto left = parseUnary();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO, TokenType::POWER})) {
        Token op = tokens[current - 1];
        auto opNode = std::make_shared<BinaryOp>();
        switch (op.type) {
            case TokenType::MULTIPLY: opNode->operation = "*"; break;
            case TokenType::DIVIDE: opNode->operation = "/"; break;
            case TokenType::MODULO: opNode->operation = "%"; break;
            case TokenType::POWER: opNode->operation = "**"; break;
            default: break;
        }
        opNode->left = left;
        opNode->right = parseUnary();
        left = opNode;
    }
    
    return left;
}

ASTNodePtr Parser::parseUnary() {
    if (match({TokenType::LOGICAL_NOT, TokenType::NOT, TokenType::VOID_NOT, TokenType::MINUS})) {
        Token op = tokens[current - 1];
        auto unary = std::make_shared<UnaryOp>();
        unary->operation = (op.type == TokenType::MINUS) ? "-" : "!";
        unary->operand = parseUnary();
        return unary;
    }
    
    return parsePrimary();
}

ASTNodePtr Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto lit = std::make_shared<Literal>();
        lit->value = tokens[current - 1].value;
        lit->dataType = "int";
        return lit;
    }
    
    if (match(TokenType::FLOAT_NUMBER)) {
        auto lit = std::make_shared<Literal>();
        lit->value = tokens[current - 1].value;
        lit->dataType = "float";
        return lit;
    }
    
    if (match(TokenType::STRING_LITERAL)) {
        auto lit = std::make_shared<Literal>();
        lit->value = tokens[current - 1].value;
        lit->dataType = "string";
        return lit;
    }
    
    if (match(TokenType::TRUE)) {
        auto lit = std::make_shared<Literal>();
        lit->value = "true";
        lit->dataType = "bool";
        return lit;
    }
    
    if (match(TokenType::FALSE)) {
        auto lit = std::make_shared<Literal>();
        lit->value = "false";
        lit->dataType = "bool";
        return lit;
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token id = tokens[current - 1];
        validateIdentifier(id.value, id.line, id.column);
        auto ident = std::make_shared<Identifier>();
        ident->name = id.value;
        return ident;
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::RPAREN)) {
            error("Expected ')' after expression", peek().line, peek().column, ErrorType::PARSER);
        }
        return expr;
    }
    
    error("Unexpected token in expression", peek().line, peek().column, ErrorType::PARSER);
    advance();
    return nullptr;
}

ASTNodePtr Parser::parse() {
    // First, tokenize everything
    Token token = scanner.nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        if (token.type != TokenType::NEWLINE) {  // Skip newlines during tokenization
            tokens.push_back(token);
        }
        token = scanner.nextToken();
    }
    tokens.push_back(token);  // Add EOF token
    
    // Add scanner errors to parser errors
    for (auto& err : scanner.getErrors()) {
        errors.push_back(err);
    }
    
    return parseProgram();
}
