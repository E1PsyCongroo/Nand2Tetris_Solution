/*Generates assembly code from the parsed VM command*/
#pragma once
#include <fstream>
#include <string>
#include "Parser.hpp"
#include <map>
using std::ofstream; using std::string;
using std::map;
class CodeWriter
{
private:
    ofstream ofs;
    int symbolIndex;
    string filename;
    string curFunctionName;
    int returnIndex;
    map<string, string> segmentMap = {
        {"local", "LCL"},
        {"this", "THIS"},
        {"that", "THAT"},
        {"argument", "ARG"},
    };
    map<string, string> arithmeticMap= {
        {"add", "M=M+D\n"},
        {"sub", "M=M-D\n"},
        {"neg", "M=-M\n"},
        {"eq", "D;JEQ\n"},
        {"gt", "D;JGT\n"},
        {"lt", "D;JLT\n"},
        {"and", "M=M&D\n"},
        {"or", "M=M|D\n"},
        {"not", "M=!M\n"},
    };
    void writePushD(); // push(D)
    void writePopD();  // D = pop()
    void writePush(string segment, int index); // push segment index
    void writePop(string segment, int index);  // pop segment index
public:
    CodeWriter(string openfile, string filename);
    ~CodeWriter();
    void writeComment(string command);  // comment
    void writeArithmetic(string command); // arithmetic&logical
    void writePushPop(Parser::CType commandType, string segment, int index); // push&pop
    void writeEnd(); // end of program
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeFunction(string functionName, int nVars);
    void writeCall(string functionName, int nArgs);
    void writeReturn();
    void writeInit();
    void setFilename(string filename);
};
