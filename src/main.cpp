#include "../include/parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>

// Helper function to convert AST to JSON
Json::Value astToJson(const ASTNodePtr& node) {
    Json::Value result(Json::objectValue);
    
    if (!node) {
        result["type"] = "null";
        return result;
    }
    
    result["type"] = node->getType();
    result["toString"] = node->toString();
    
    return result;
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
