#pragma once

#include <fstream>
#include <string>
#include <set>

using std::ifstream; using std::string;
using std::set;

class JackTokenizer {
public:

    enum class TokenType { KEYWORD, SYMBOL, INTEGER, STRING, IDENTIFIER };

    JackTokenizer(string filename);
    ~JackTokenizer();

    bool hasMoreTokens();
    void advance();
    TokenType tokenType();
    string curToken();
    void setFile(string filename);

private:
    ifstream source;
    string _curToken;
    TokenType _tokenType;

    set<string> keywordSet = {
        "class", "constructor", "function", "method", "field",
        "static", "var", "int", "char", "boolean",
        "void", "true", "false", "null", "this",
        "let", "do", "if", "else", "while", "return",
    };
    set<char> symbol = {
        '{', '}', '(', ')','[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~',
    };

    bool getToken();
    void ignoreComment(char ch);
};