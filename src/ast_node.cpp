#include "../include/ast_node.h"
#include <sstream>

std::string Program::toString() const {
    std::stringstream ss;
    ss << "Program(" << name << ")";
    return ss.str();
}

std::string Declaration::toString() const {
    std::stringstream ss;
    ss << "Declaration(" << dataType << " [";
    for (size_t i = 0; i < identifiers.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << identifiers[i];
        if (i < initializers.size() && initializers[i] != nullptr) {
            ss << " = (expr)";
        }
    }
    ss << "])";
    return ss.str();
}

std::string Assignment::toString() const {
    std::stringstream ss;
    ss << "Assignment(" << identifier << " = ...)";
    return ss.str();
}

std::string BinaryOp::toString() const {
    std::stringstream ss;
    ss << "BinaryOp(" << operation << ")";
    return ss.str();
}

std::string UnaryOp::toString() const {
    std::stringstream ss;
    ss << "UnaryOp(" << operation << ")";
    return ss.str();
}

std::string Literal::toString() const {
    std::stringstream ss;
    ss << "Literal(" << value << " : " << dataType << ")";
    return ss.str();
}

std::string Identifier::toString() const {
    std::stringstream ss;
    ss << "Identifier(" << name << ")";
    return ss.str();
}

std::string FunctionCall::toString() const {
    std::stringstream ss;
    ss << "FunctionCall(" << functionName << ")";
    return ss.str();
}

std::string IfStatement::toString() const {
    return "IfStatement(...)";
}

std::string WhileLoop::toString() const {
    return "WhileLoop(...)";
}

std::string ForLoop::toString() const {
    return "ForLoop(...)";
}

std::string ReturnStatement::toString() const {
    return "ReturnStatement(...)";
}

std::string Function::toString() const {
    std::stringstream ss;
    ss << "Function(" << name << " -> " << returnType << ")";
    return ss.str();
}
