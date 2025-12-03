#include "../include/symbol_table.h"
#include <stdexcept>

void SymbolTable::addSymbol(const std::string& name, const std::string& type, int line, int column) {
    if (symbols.find(name) != symbols.end()) {
        throw std::runtime_error("Symbol '" + name + "' already declared");
    }
    symbols[name] = std::make_shared<Symbol>(name, type, line, column);
}

bool SymbolTable::exists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

std::shared_ptr<Symbol> SymbolTable::getSymbol(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        throw std::runtime_error("Symbol '" + name + "' not declared");
    }
    return it->second;
}

std::string SymbolTable::getType(const std::string& name) const {
    return getSymbol(name)->type;
}
