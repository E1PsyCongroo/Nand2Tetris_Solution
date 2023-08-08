#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

using std::string; using std::vector;
using std::unordered_map; using std::tuple;

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();

    enum class SymbolKind{STATIC, FIELD, ARG, VAR};

    void reset();
    void define(string name, string type, SymbolKind kind);
    int varCount(SymbolKind kind);
    SymbolKind kindOf(string name);
    string typeOf(string name);
    int indexOf(string name);
    bool exist(string name);

private:
    using Symbol = tuple<string, string, int>;
    unordered_map<SymbolKind, vector<Symbol>> _symbolTable;
    unordered_map<SymbolKind, int> _symbolCount;
};