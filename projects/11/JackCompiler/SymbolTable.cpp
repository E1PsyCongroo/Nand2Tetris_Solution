#include "SymbolTable.hpp"
#include <stdexcept>

SymbolTable::SymbolTable() {
    _symbolTable = {
        {SymbolKind::STATIC, vector<Symbol>{}},
        {SymbolKind::FIELD, vector<Symbol>{}},
        {SymbolKind::ARG, vector<Symbol>{}},
        {SymbolKind::VAR, vector<Symbol>{}},
    };
    _symbolCount = {
        {SymbolKind::STATIC, 0},
        {SymbolKind::FIELD, 0},
        {SymbolKind::ARG, 0},
        {SymbolKind::VAR, 0},
    };
}

SymbolTable::~SymbolTable() { }

void SymbolTable::reset() {
    _symbolTable = {
        {SymbolKind::STATIC, vector<Symbol>{}},
        {SymbolKind::FIELD, vector<Symbol>{}},
        {SymbolKind::ARG, vector<Symbol>{}},
        {SymbolKind::VAR, vector<Symbol>{}},
    };
    _symbolCount = {
        {SymbolKind::STATIC, 0},
        {SymbolKind::FIELD, 0},
        {SymbolKind::ARG, 0},
        {SymbolKind::VAR, 0},
    };
}

void SymbolTable::define(string name, string type, SymbolKind kind) {
    _symbolTable[kind].push_back({name, type, _symbolCount[kind]++});
}

int SymbolTable::varCount(SymbolKind kind) {
    return _symbolCount[kind];
}

SymbolTable::SymbolKind SymbolTable::kindOf(string name) {
    for (auto [key, symbols] : _symbolTable) {
        for (auto symbol : symbols) {
            if (std::get<0>(symbol) == name) {
                return key;
            }
        }
    }
    throw std::runtime_error("SymbolTable don't have \"" + name + "\" symbol");
}

string SymbolTable::typeOf(string name) {
    for (auto [_, symbols] : _symbolTable) {
        for (auto symbol : symbols) {
            if (std::get<0>(symbol) == name) {
                return std::get<1>(symbol);
            }
        }
    }
    throw std::runtime_error("SymbolTable don't have \"" + name + "\" symbol");
}

int SymbolTable::indexOf(string name) {
    for (auto [_, symbols] : _symbolTable) {
        for (auto symbol : symbols) {
            if (std::get<0>(symbol) == name) {
                return std::get<2>(symbol);
            }
        }
    }
    throw std::runtime_error("SymbolTable don't have \"" + name + "\" symbol");
}

bool SymbolTable::exist(string name) {
    for (auto [_, symbols] : _symbolTable) {
        for (auto symbol : symbols) {
            if (std::get<0>(symbol) == name) {
                return true;
            }
        }
    }
    return false;
}