#include "Parser.hpp"
#include <algorithm>
#include <iostream>
using std::string; using std::ifstream;
using std::cout;

Parser::Parser(string filename): source{filename, std::ios::in}, curCommand{} {}

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
    if (curCommand.find("push") == 0) {
        return CType::C_PUSH;
    }
    else if (curCommand.find("pop") == 0) {
        return CType::C_POP;
    }
    else if (ALCommand.find(curCommand) != ALCommand.end()) {
        return CType::C_ARITHMETIC;
    }
    else {
        return CType::EMPTY;
    }
}

string Parser::arg1() {
    CType type = commandType();
    if (type == CType::C_PUSH || type == CType::C_POP) {
        auto first = curCommand.find_first_of(' ');
        auto last = curCommand.find_last_of(' ');
        return curCommand.substr(first+1, last-first-1);
    }
    else {
        return curCommand;
    }
}

int Parser::arg2() {
    CType type = commandType();
    if (type == CType::C_PUSH || type == CType::C_POP) {
        auto first = curCommand.find_last_of(' ');
        return std::stoi(curCommand.substr(first+1));
    }
    return 0;
}

void Parser::reset() {
    source.clear();
    source.seekg(0, std::ios_base::beg);
    curCommand="";
}

void Parser::getCommand() {
    string nextCommand;
    while (nextCommand.empty() && hasMoreLines()) {
        std::getline(source, nextCommand, '\n');
        //ignore Space and Comments
        auto commentPos = nextCommand.find("//");
        if (commentPos != std::string::npos) {
            nextCommand.erase(commentPos);
        }
        size_t first = nextCommand.find_first_not_of(' ');
        size_t last = nextCommand.find_last_not_of(' ');
        if (first != std::string::npos) {
            nextCommand = nextCommand.substr(first, last - first + 1);
        }
    }
    curCommand = nextCommand;
}

string Parser::curCom() {
    return curCommand;
}