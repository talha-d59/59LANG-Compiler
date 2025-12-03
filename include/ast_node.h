#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

// Forward declarations
struct ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;
using ASTNodeList = std::vector<ASTNodePtr>;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual std::string getType() const = 0;
    virtual std::string toString() const = 0;
};

struct Program : ASTNode {
    std::string name;
    ASTNodeList declarations;
    ASTNodeList statements;
    
    std::string getType() const override { return "Program"; }
    std::string toString() const override;
};

struct Declaration : ASTNode {
    std::string dataType;
    std::vector<std::string> identifiers;
    std::vector<ASTNodePtr> initializers;  // Optional initialization expressions (nullptr if not initialized)
    
    std::string getType() const override { return "Declaration"; }
    std::string toString() const override;
};

struct Assignment : ASTNode {
    std::string identifier;
    ASTNodePtr expression;
    
    std::string getType() const override { return "Assignment"; }
    std::string toString() const override;
};

struct BinaryOp : ASTNode {
    std::string operation;
    ASTNodePtr left;
    ASTNodePtr right;
    
    std::string getType() const override { return "BinaryOp"; }
    std::string toString() const override;
};

struct UnaryOp : ASTNode {
    std::string operation;
    ASTNodePtr operand;
    
    std::string getType() const override { return "UnaryOp"; }
    std::string toString() const override;
};

struct Literal : ASTNode {
    std::string value;
    std::string dataType;
    
    std::string getType() const override { return "Literal"; }
    std::string toString() const override;
};

struct Identifier : ASTNode {
    std::string name;
    
    std::string getType() const override { return "Identifier"; }
    std::string toString() const override;
};

struct FunctionCall : ASTNode {
    std::string functionName;
    ASTNodeList arguments;
    
    std::string getType() const override { return "FunctionCall"; }
    std::string toString() const override;
};

struct IfStatement : ASTNode {
    ASTNodePtr condition;
    ASTNodeList thenBranch;
    ASTNodeList elseBranch;
    
    std::string getType() const override { return "IfStatement"; }
    std::string toString() const override;
};

struct WhileLoop : ASTNode {
    ASTNodePtr condition;
    ASTNodeList body;
    
    std::string getType() const override { return "WhileLoop"; }
    std::string toString() const override;
};

struct ForLoop : ASTNode {
    ASTNodePtr initialization;
    ASTNodePtr condition;
    ASTNodePtr increment;
    ASTNodeList body;
    
    std::string getType() const override { return "ForLoop"; }
    std::string toString() const override;
};

struct ReturnStatement : ASTNode {
    ASTNodePtr expression;
    
    std::string getType() const override { return "ReturnStatement"; }
    std::string toString() const override;
};

struct Function : ASTNode {
    std::string name;
    std::string returnType;
    ASTNodeList parameters;
    ASTNodeList body;
    
    std::string getType() const override { return "Function"; }
    std::string toString() const override;
};

#endif // AST_NODE_H
