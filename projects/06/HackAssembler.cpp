#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <fstream>
#include "Parser.hpp"
#include "Code.hpp"
#include "SymbolTable.hpp"

using std::cout; using std::endl;
using std::string; using std::vector;
using std::ofstream;

string getCode(Parser&, SymbolTable&);

int main(const int argc, const char* argv[]){
    if (argc != 2) {
        cout << "Usage: Assembler filename\n";
        exit(1);
    }
    string filename = argv[1];
    Parser parser(filename);
    SymbolTable symbolTable(filename);
    auto dotIndex = filename.find_last_of(".");
    string outFilename =filename.substr(0, dotIndex+1) + "hack";
    ofstream ofs(outFilename);
    while (parser.hasMoreLines()) {
        parser.advance();
        string code = getCode(parser, symbolTable);
        if (!code.empty()) {
            ofs << code << '\n';
        }
    }
    ofs.close();
    return 0;
}

string getCode(Parser& parser, SymbolTable& symbolTable) {
    if (parser.instructionType() == Parser::Type::A_INSTRUCTION) {
        if (symbolTable.contains(parser.symbol())) {
            return "0" + Code::dTob(symbolTable.getAddress(parser.symbol()));
        }
        return "0" + Code::dTob(std::atoi(parser.symbol().c_str()));
    }
    else if (parser.instructionType() == Parser::Type::C_INSTRUCTION) {
        return "111" + Code::comp(parser.comp()) + Code::dest(parser.dest()) + Code::jump(parser.jump());
    }
    return "";
}