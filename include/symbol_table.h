#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <memory>

struct Symbol {
    std::string name;
    std::string type;
    int line;
    int column;
    bool initialized;
    
    Symbol(const std::string& n, const std::string& t, int l, int c, bool init = false)
        : name(n), type(t), line(l), column(c), initialized(init) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    
public:
    SymbolTable() = default;
    
    void addSymbol(const std::string& name, const std::string& type, int line, int column);
    bool exists(const std::string& name) const;
    std::shared_ptr<Symbol> getSymbol(const std::string& name) const;
    std::string getType(const std::string& name) const;
    
    const std::unordered_map<std::string, std::shared_ptr<Symbol>>& getAllSymbols() const {
        return symbols;
    }
};

#endif // SYMBOL_TABLE_H
