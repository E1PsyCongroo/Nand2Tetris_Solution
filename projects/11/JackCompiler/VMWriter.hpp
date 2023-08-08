#pragma once
#include <fstream>
#include <string>
#include <map>

using std::ofstream; using std::string;
using std::map;

class VMWriter {
public:
    VMWriter(string filename);
    ~VMWriter();

    void writePush(string segment, int index);
    void writePop(string segment, int index);
    void writeArithmetic(string command);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nVars);
    void writeReturn();

    void setFile(string filename);

private:
    ofstream ofs;
};