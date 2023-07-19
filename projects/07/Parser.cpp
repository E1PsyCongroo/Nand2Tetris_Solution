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
        getCommand();
    }
}

Parser::CType Parser::commandType() {
    if (curCommand.find("push") != std::string::npos) {
        return CType::C_PUSH;
    }
    else if (curCommand.find("pop") != std::string::npos) {
        return CType::C_POP;
    }
    else {
        return
    }
}

string Parser::arg1();

int Parser::arg2();

void Parser::reset() {
    source.clear();
    source.seekg(0, std::ios_base::beg);
    curCommand="";
}

void Parser::getCommand() {
    string nextCommand;
    while (nextCommand.empty() && hasMoreLines()) {
        std::getline(source, nextCommand);
        //ignore Space and Comments
        for (auto it=nextCommand.begin(); it != nextCommand.end(); it++) {
            if (*it == '/' && it + 1 != nextCommand.end() && *(it+1) == '/') {
                nextCommand.erase(it, nextCommand.end());
                break;
            }
        }
        size_t first = nextCommand.find_first_not_of(' ');
        size_t last = nextCommand.find_last_not_of(' ');
        nextCommand = nextCommand.substr(first, last - first + 1);
    }
    curCommand = nextCommand;
}