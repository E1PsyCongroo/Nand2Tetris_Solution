#include <iostream>
#include <string>
#include <fstream>
#include "Parser.hpp"
#include "CodeWriter.hpp"
using std::cout; using std::endl;
using std::string; using std::ofstream;
using std::ifstream;

int main(const int argc, const char* argv[]) {

    if (argc != 2) {
        cout << "Usage: VMTranslator filename\n";
        exit(1);
    }
    string filename = argv[1];
    Parser parser(filename);
    auto dotIndex = filename.find_last_of(".");
    string outFilename =filename.substr(0, dotIndex+1) + "asm";
    CodeWriter codeWriter(outFilename);
    while (parser.hasMoreLines()) {
        parser.advance();
        cout << "curCommand: " << parser.curCom() << ", Type: " << int(parser.commandType())
        <<  ", arg1: " << parser.arg1() << ", arg2: " << parser.arg2() << '\n';
        if (parser.commandType() == Parser::CType::EMPTY) {
            break;
        }
        else if (parser.commandType() == Parser::CType::C_PUSH | parser.commandType() == Parser::CType::C_POP) {
            codeWriter.writeComment(parser.curCom());
            codeWriter.writerPushPop(parser.commandType(), parser.arg1(), parser.arg2());
        }
        else if (parser.commandType() == Parser::CType::C_ARITHMETIC) {
            codeWriter.writeComment(parser.curCom());
            codeWriter.writeArithmetic(parser.arg1());
        }
        else {
            break;
        }
    }
    codeWriter.writerEnd();
    return 0;
}