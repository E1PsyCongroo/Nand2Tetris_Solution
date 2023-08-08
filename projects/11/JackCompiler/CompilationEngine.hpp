#pragma once

#include <string>
#include <fstream>
#include <map>
#include <set>
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "VMWriter.hpp"

using std::ofstream; using std::map;
using std::set;

class CompilationEngine {
public:
    CompilationEngine(string input, string output);
    ~CompilationEngine();

    void setFile(string input, string ouput);

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();

private:
    JackTokenizer _jackTokenizer;
    VMWriter _VMWriter;
    SymbolTable _classVar;
    SymbolTable _subroutineVar;
    string _curClass;
    inline static int labelCount{0};

    const map<string, string> arithmeticMap = {
        {"+", "add"}, {"-", "sub"},
        {"<", "lt"}, {">", "gt"},
        {"=", "eq"}, {"&", "and"},
        {"|", "or"}, {"~", "not"},
    };
    map<JackTokenizer::TokenType, string> typeMap = {
        {JackTokenizer::TokenType::IDENTIFIER, "identifier"},
        {JackTokenizer::TokenType::KEYWORD, "keyword"},
        {JackTokenizer::TokenType::SYMBOL, "symbol"},
        {JackTokenizer::TokenType::INTEGER, "integerConstant"},
        {JackTokenizer::TokenType::STRING, "stringConstant"},
    };
    const set<string> opSet = {"+", "-", "*", "/", "&", "|", "<", ">", "="};

    inline string getToken() {
        string result = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        return result;
    }
};