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
    else if (curCommand.find("label") == 0) {
        return CType::C_LABEL;
    }
    else if (curCommand.find("goto") == 0) {
        return CType::C_GOTO;
    }
    else if (curCommand.find("call") == 0) {
        return CType::C_CALL;
    }
    else if (curCommand.find("if-goto") == 0) {
        return CType::C_IF;
    }
    else if (curCommand.find("function") == 0) {
        return CType::C_FUNCTION;
    }
    else if (curCommand.find("return") == 0) {
        return CType::C_RETURN;
    }
    else {
        return CType::EMPTY;
    }
}

string Parser::arg1() {
    const set<CType> onlyCmd = {CType::C_ARITHMETIC, CType::C_RETURN};
    const CType type = commandType();
    if (type == CType::EMPTY) {
        // throw std::runtime_error("Empty cmd don't have arg1()!");
        return "";
    }
    else if (onlyCmd.count(type)) {
        return curCommand;
    }
    else {
        string temp = curCommand;
        for (int i = 0; i < 1; i ++) {
            auto start = temp.find_first_of(' ');
            temp = temp.substr(start);
            start = temp.find_first_not_of(' ');
            temp = temp.substr(start);
        }

        auto last = temp.find_first_of(' ');
        if (last != std::string::npos) {
            return temp.substr(0, last);
        }
        else {
            return temp;
        }
    }
}

int Parser::arg2() {
    const CType type = commandType();
    if (type == CType::C_PUSH || type == CType::C_POP || type == CType::C_FUNCTION || type == CType::C_CALL) {
        string temp = curCommand;
        for (int i = 0; i < 2; i ++) {
            auto start = temp.find_first_of(' ');
            temp = temp.substr(start);
            start = temp.find_first_not_of(' ');
            temp = temp.substr(start);
        }

        return std::stoi(temp);
    }
    else {
        // throw std::runtime_error("\"" + curCommand + "\"" + " cmd don't have arg2()!");
        return 0;
    }
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