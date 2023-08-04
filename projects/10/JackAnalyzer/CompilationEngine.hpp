#pragma once

#include <string>
#include <fstream>
#include <map>
#include <set>
#include "JackTokenizer.hpp"

using std::ofstream; using std::map;
using std::set;

class CompilationEngine {
public:
    CompilationEngine(string input, string output);
    ~CompilationEngine();

    void setFile(string input, string ouput);

    void compileTokens();

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();

private:
    JackTokenizer _jackTokenizer;
    ofstream ofs;

    map<string, string> symbolMap = {
        {"<", "&lt;"}, {">", "&gt;"},
        {"\"", "&quot;"}, {"&", "&amp;"},
    };
    map<JackTokenizer::TokenType, string> typeMap = {
        {JackTokenizer::TokenType::IDENTIFIER, "identifier"},
        {JackTokenizer::TokenType::KEYWORD, "keyword"},
        {JackTokenizer::TokenType::SYMBOL, "symbol"},
        {JackTokenizer::TokenType::INTEGER, "integerConstant"},
        {JackTokenizer::TokenType::STRING, "stringConstant"},
    };
    set<string> opSet = {"+", "-", "*", "/", "&", "|", "<", ">", "="};
    set<string> unaryOpSet = {"-", "~"};

    void compileToken();

    inline string XMLstart(string token) {
        return "<" + token + ">";
    }

    inline string XMLend(string token) {
        return "</" + token + ">";
    }

    inline string getToken() {
        string result = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        if (symbolMap.count(result)) {
            return symbolMap[result];
        }
        else {
            return result;
        }
    }
};