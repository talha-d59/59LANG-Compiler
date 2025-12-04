#include "../include/parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>

// Helper function to convert AST to JSON (recursive tree representation)
Json::Value makeAstNode(const ASTNodePtr& n) {
    Json::Value obj(Json::objectValue);
    if (!n) {
        obj["label"] = "<null>";
        return obj;
    }

    std::string t = n->getType();
    if (t == "Program") {
        auto p = std::static_pointer_cast<Program>(n);
        obj["label"] = "PROGRAM";
        Json::Value children(Json::arrayValue);
        for (auto& decl : p->declarations) children.append(makeAstNode(decl));
        for (auto& stmt : p->statements) children.append(makeAstNode(stmt));
        obj["children"] = children;
        return obj;
    }

    if (t == "Declaration") {
        auto d = std::static_pointer_cast<Declaration>(n);
        Json::Value children(Json::arrayValue);
        for (size_t i = 0; i < d->identifiers.size(); ++i) {
            std::string name = d->identifiers[i];
            std::string label = "VAR_DECL(" + d->dataType + " " + name + ")";
            Json::Value idNode(Json::objectValue);
            idNode["label"] = label;
            if (i < d->initializers.size() && d->initializers[i] != nullptr) {
                Json::Value sub(Json::arrayValue);
                sub.append(makeAstNode(d->initializers[i]));
                idNode["children"] = sub;
            }
            children.append(idNode);
        }
        obj["label"] = "DECL";
        obj["children"] = children;
        return obj;
    }

    if (t == "Assignment") {
        auto a = std::static_pointer_cast<Assignment>(n);
        obj["label"] = std::string("ASSIGN(") + a->identifier + ")";
        Json::Value children(Json::arrayValue);
        if (a->expression) children.append(makeAstNode(a->expression));
        obj["children"] = children;
        return obj;
    }

    if (t == "BinaryOp") {
        auto b = std::static_pointer_cast<BinaryOp>(n);
        obj["label"] = std::string("EXPR(") + b->operation + ")";
        Json::Value children(Json::arrayValue);
        if (b->left) children.append(makeAstNode(b->left));
        if (b->right) children.append(makeAstNode(b->right));
        obj["children"] = children;
        return obj;
    }

    if (t == "UnaryOp") {
        auto u = std::static_pointer_cast<UnaryOp>(n);
        obj["label"] = std::string("UNARY(") + u->operation + ")";
        Json::Value children(Json::arrayValue);
        if (u->operand) children.append(makeAstNode(u->operand));
        obj["children"] = children;
        return obj;
    }

    if (t == "Literal") {
        auto l = std::static_pointer_cast<Literal>(n);
        obj["label"] = l->value;
        return obj;
    }

    if (t == "Identifier") {
        auto id = std::static_pointer_cast<Identifier>(n);
        obj["label"] = id->name;
        return obj;
    }

    if (t == "FunctionCall") {
        auto f = std::static_pointer_cast<FunctionCall>(n);
        obj["label"] = std::string("CALL(") + f->functionName + ")";
        Json::Value children(Json::arrayValue);
        for (auto& a : f->arguments) children.append(makeAstNode(a));
        obj["children"] = children;
        return obj;
    }

    if (t == "IfStatement") {
        auto iff = std::static_pointer_cast<IfStatement>(n);
        obj["label"] = "IF";
        Json::Value children(Json::arrayValue);
        if (iff->condition) children.append(makeAstNode(iff->condition));
        Json::Value thenNode(Json::objectValue);
        thenNode["label"] = "THEN";
        Json::Value thenChildren(Json::arrayValue);
        for (auto& s : iff->thenBranch) thenChildren.append(makeAstNode(s));
        thenNode["children"] = thenChildren;
        children.append(thenNode);
        if (!iff->elseBranch.empty()) {
            Json::Value elseNode(Json::objectValue);
            elseNode["label"] = "ELSE";
            Json::Value elseChildren(Json::arrayValue);
            for (auto& s : iff->elseBranch) elseChildren.append(makeAstNode(s));
            elseNode["children"] = elseChildren;
            children.append(elseNode);
        }
        obj["children"] = children;
        return obj;
    }

    if (t == "WhileLoop") {
        auto w = std::static_pointer_cast<WhileLoop>(n);
        obj["label"] = "WHILE";
        Json::Value children(Json::arrayValue);
        if (w->condition) children.append(makeAstNode(w->condition));
        Json::Value body(Json::objectValue);
        body["label"] = "BODY";
        Json::Value bodyChildren(Json::arrayValue);
        for (auto& s : w->body) bodyChildren.append(makeAstNode(s));
        body["children"] = bodyChildren;
        children.append(body);
        obj["children"] = children;
        return obj;
    }

    if (t == "ForLoop") {
        auto f = std::static_pointer_cast<ForLoop>(n);
        obj["label"] = "FOR";
        Json::Value children(Json::arrayValue);
        if (f->initialization) children.append(makeAstNode(f->initialization));
        if (f->condition) children.append(makeAstNode(f->condition));
        if (f->increment) children.append(makeAstNode(f->increment));
        Json::Value body(Json::objectValue);
        body["label"] = "BODY";
        Json::Value bodyChildren(Json::arrayValue);
        for (auto& s : f->body) bodyChildren.append(makeAstNode(s));
        body["children"] = bodyChildren;
        children.append(body);
        obj["children"] = children;
        return obj;
    }

    if (t == "ReturnStatement") {
        auto r = std::static_pointer_cast<ReturnStatement>(n);
        obj["label"] = "RETURN";
        Json::Value children(Json::arrayValue);
        if (r->expression) children.append(makeAstNode(r->expression));
        obj["children"] = children;
        return obj;
    }

    if (t == "Function") {
        auto fn = std::static_pointer_cast<Function>(n);
        obj["label"] = std::string("FUNC(") + fn->name + ")";
        Json::Value children(Json::arrayValue);
        for (auto& p : fn->parameters) children.append(makeAstNode(p));
        for (auto& s : fn->body) children.append(makeAstNode(s));
        obj["children"] = children;
        return obj;
    }

    // Fallback: include toString as label
    obj["label"] = n->toString();
    return obj;
}

Json::Value astToJson(const ASTNodePtr& node) {
    return makeAstNode(node);
}

// Helper function to convert errors to JSON
Json::Value errorsToJson(const std::vector<std::shared_ptr<Error>>& errors) {
    Json::Value result(Json::arrayValue);
    
    for (const auto& err : errors) {
        Json::Value errObj(Json::objectValue);
        errObj["message"] = err->message;
        errObj["line"] = err->line;
        errObj["column"] = err->column;
        errObj["type"] = err->typeToString();
        result.append(errObj);
    }
    
    return result;
}

// Helper to convert tokens to JSON
Json::Value tokensToJson(const std::vector<Token>& tokens) {
    Json::Value arr(Json::arrayValue);
    for (const auto& t : tokens) {
        Json::Value obj(Json::objectValue);
        obj["type"] = t.typeToString();
        obj["value"] = t.value;
        obj["line"] = t.line;
        obj["column"] = t.column;
        arr.append(obj);
    }
    return arr;
}

// Helper function to convert symbol table to JSON
Json::Value symbolTableToJson(const SymbolTable& table) {
    Json::Value result(Json::objectValue);
    
    for (const auto& pair : table.getAllSymbols()) {
        Json::Value symbolObj(Json::objectValue);
        symbolObj["name"] = pair.second->name;
        symbolObj["type"] = pair.second->type;
        symbolObj["line"] = pair.second->line;
        symbolObj["column"] = pair.second->column;
        result[pair.first] = symbolObj;
    }
    
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [--json]" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    bool outputJson = (argc > 2 && std::string(argv[2]) == "--json");
    
    // Read source file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();
    
    // Parse
    Parser parser(source);
    ASTNodePtr ast = parser.parse();
    
    if (outputJson) {
        Json::Value output(Json::objectValue);
        output["errors"] = errorsToJson(parser.getErrors());
        output["symbolTable"] = symbolTableToJson(parser.getSymbolTable());
        output["hasErrors"] = parser.hasErrors();
        output["errorCount"] = static_cast<int>(parser.getErrors().size());
        output["tokens"] = tokensToJson(parser.getTokens());
        output["ast"] = astToJson(ast);
        
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "  ";
        std::cout << Json::writeString(writer, output);
    } else {
        if (!parser.hasErrors()) {
            std::cout << "Parsing successful!" << std::endl;
            std::cout << "Symbol Table:" << std::endl;
            for (const auto& pair : parser.getSymbolTable().getAllSymbols()) {
                std::cout << "  " << pair.first << " : " << pair.second->type << std::endl;
            }
        } else {
            std::cout << "Parsing completed with " << parser.getErrors().size() << " error(s):" << std::endl;
            for (const auto& err : parser.getErrors()) {
                std::cout << "  " << err->toString() << std::endl;
            }
        }
    }
    
    return parser.hasErrors() ? 1 : 0;
}
