#include "Parser.hpp"
#include <algorithm>
#include <iostream>
using std::string; using std::ifstream;

Parser::Parser(string filename): source{filename}, curInstruction{} {}

Parser::~Parser() {
    source.close();
}

bool Parser::hasMoreLines() {
    return !source.eof();
}

void Parser::advance() {
    if (hasMoreLines()) {
        getInstruction();
    }
}

CType Parser::commandType();

string Parser::arg1();

int Parser::arg2();

void Parser::reset() {
    source.clear();
    source.seekg(0, std::ios_base::beg);
    curInstruction="";
}

void Parser::getInstruction() {
    string nextInstruciton;
    while (nextInstruciton.empty() && hasMoreLines()) {
        std::getline(source, nextInstruciton);
        //ignore Space and Comment
        nextInstruciton.erase(std::remove_if(nextInstruciton.begin(), nextInstruciton.end(), ::isspace), nextInstruciton.end());
        for (auto it=nextInstruciton.begin(); it != nextInstruciton.end(); it++) {
            if (*it == '/' && it + 1 != nextInstruciton.end() && *(it+1) == '/') {
                nextInstruciton.erase(it, nextInstruciton.end());
                break;
            }
        }
    }
    curInstruction = nextInstruciton;
}