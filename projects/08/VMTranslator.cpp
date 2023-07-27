#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "Parser.hpp"
#include "CodeWriter.hpp"
using std::cout; using std::endl;
using std::string; using std::ofstream;
using std::ifstream;

using namespace std::filesystem;

void writer(Parser& parser, CodeWriter& codeWriter);

int main(const int argc, const char* argv[]) {

    if (argc != 2) {
        cout << "Usage: VMTranslator filename\n";
        exit(1);
    }
    path inputPath = argv[1];
    cout << "Input PATH: " << inputPath << '\n';
    if (inputPath.has_filename()) {
        Parser parser(inputPath.string());
        CodeWriter codeWriter(inputPath.replace_extension("asm").string(), inputPath.stem().string());
        writer(parser, codeWriter);
        codeWriter.writeEnd();
    }
    else {
        string filename = inputPath.parent_path().filename().string() + ".asm";
        CodeWriter codeWriter(inputPath.string() + "\\" + filename, "Sys");
        codeWriter.writeComment("SP=256");
        codeWriter.writeComment("call Sys.init 0");
        codeWriter.writeInit();
        for (auto& dir_entry : directory_iterator{inputPath}) {
            if (dir_entry.is_regular_file() && dir_entry.path().extension().string() == ".vm") {
                cout << "----------------------------------\n";
                cout << "Path: " << dir_entry.path() << '\n';
                cout << "----------------------------------\n";
                Parser parser{dir_entry.path().string()};
                codeWriter.setFilename(dir_entry.path().stem().string());
                writer(parser, codeWriter);
            }
        }
    }
    return 0;
}

void writer(Parser& parser, CodeWriter& codeWriter) {
    while (parser.hasMoreLines()) {
        parser.advance();
        cout << "curCommand: " << parser.curCom() << ", Type: " << int(parser.commandType())
        <<  ", arg1: " << parser.arg1() << ", arg2: " << parser.arg2() << '\n';
        if (parser.commandType() == Parser::CType::EMPTY) {
            break;
        }
        else {
            codeWriter.writeComment(parser.curCom());
            if (parser.commandType() == Parser::CType::C_PUSH | parser.commandType() == Parser::CType::C_POP) {
                codeWriter.writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
            }
            else if (parser.commandType() == Parser::CType::C_ARITHMETIC) {
                codeWriter.writeArithmetic(parser.arg1());
            }
            else if (parser.commandType() == Parser::CType::C_CALL) {
                codeWriter.writeCall(parser.arg1(), parser.arg2());
            }
            else if (parser.commandType() == Parser::CType::C_FUNCTION) {
                codeWriter.writeFunction(parser.arg1(), parser.arg2());
            }
            else if (parser.commandType() == Parser::CType::C_GOTO) {
                codeWriter.writeGoto(parser.arg1());
            }
            else if (parser.commandType() == Parser::CType::C_IF) {
                codeWriter.writeIf(parser.arg1());
            }
            else if (parser.commandType() == Parser::CType::C_LABEL) {
                codeWriter.writeLabel(parser.arg1());
            }
            else if (parser.commandType() == Parser::CType::C_RETURN) {
                codeWriter.writeReturn();
            }
        }
    }
}