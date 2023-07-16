/*handles symbols*/
#include "SymbolTable.hpp"
#include "Parser.hpp"
#include <fstream>
#include <iostream>

using std::string; using std::map;
using std::ifstream;

SymbolTable::SymbolTable(string filename):
    _table{
        {"R0", 0},
        {"R1", 1},
        {"R2", 2},
        {"R3", 3},
        {"R4", 4},
        {"R5", 5},
        {"R6", 6},
        {"R7", 7},
        {"R8", 8},
        {"R9", 9},
        {"R10", 10},
        {"R11", 11},
        {"R12", 12},
        {"R13", 13},
        {"R14", 14},
        {"R15", 15},
        {"SCREEN", 16384},
        {"KBD", 24576},
        {"SP", 0},
        {"LCL", 1},
        {"ARG", 2},
        {"THIS", 3},
        {"THAT", 4},
    }
{
    Parser parser(filename);
    int count = 0;
    while (parser.hasMoreLines()) {
        parser.advance();
        if (parser.instructionType() == Parser::Type::L_INSTRUCTION) {
            std::cout << parser.symbol() << " " << count << '\n';
            addEntry(parser.symbol(), count--);
        }
        count++;
    }
    parser.reset();
    count = 16;
    while (parser.hasMoreLines()) {
        parser.advance();
        if (parser.instructionType() == Parser::Type::A_INSTRUCTION) {
            if (!isNumeric(parser.symbol()) && !contains(parser.symbol())) {
                std::cout << parser.symbol() << " " << count << '\n';
                addEntry(parser.symbol(), count++);
            }
        }
    }
}

bool SymbolTable::contains(string symbol) {
    return _table.count(symbol);
}

int SymbolTable::getAddress(string symbol) {
    return _table.at(symbol);
}

void SymbolTable::addEntry(string symbol, int addr) {
    _table.insert({symbol, addr});
}

bool SymbolTable::isNumeric(const string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isdigit(c) && c != '-' && c != '+') {
            return false;
        }
    }
    return true;
}