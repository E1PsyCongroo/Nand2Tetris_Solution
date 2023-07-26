/*Generates assembly code from the parsed VM command*/
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
    void writerPushD(); // push(D)
    void writerPopD();  // D = pop()
    void writerPush(string segment, int index); // push segment index
    void writerPop(string segment, int index);  // pop segment index
public:
    CodeWriter(string filename);
    ~CodeWriter();
    void writeComment(string command);  // comment
    void writeArithmetic(string command); // arithmetic&logical
    void writerPushPop(Parser::CType commandType, string segment, int index); // push&pop
    void writerEnd(); // end of program
};
