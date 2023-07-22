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
    void writerPushD();
    void writerPopD();
    void writerPush(string segment, int index);
    void writerPop(string segment, int index);
public:
    CodeWriter(string filename);
    ~CodeWriter();
    void writeArithmetic(string command);
    void writerPushPop(Parser::CType commandType, string segment, int index);
    void writerEnd();
};
