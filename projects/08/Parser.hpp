/*reads and parses an instruction*/
#pragma once

#include <fstream>
#include <string>
#include <set>
using std::ifstream; using std::string;
using std::set;
class Parser
{
    public:
    Parser(string filename);
    ~Parser();
    enum class CType{C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL, EMPTY};
    bool hasMoreLines();
    void advance();
    CType commandType();
    string arg1();
    int arg2();
    void reset();
    string curCom();

    private:
    ifstream source;
    string curCommand;
    void getCommand();
    const set<string> ALCommand = {"add", "sub", "neg", "eq", "lt", "gt", "and", "or", "not"};
};