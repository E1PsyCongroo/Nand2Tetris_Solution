/*reads and parses an instruction*/
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

Parser::Type Parser::instructionType() {
    if (curInstruction.empty()) {
        return Type::EMPTY;
    }
    else if (curInstruction[0] == '@') {
        return Type::A_INSTRUCTION;
    }
    else if (curInstruction[0] == '(' && curInstruction[curInstruction.length()-1] == ')') {
        return Type::L_INSTRUCTION;
    }
    else {
        return Type::C_INSTRUCTION;
    }
}

string Parser::symbol() {
    if (instructionType() == Type::A_INSTRUCTION) {
        return curInstruction.substr(1);
    }
    else if (instructionType() == Type::L_INSTRUCTION) {
        return curInstruction.substr(1, curInstruction.length()-2);
    }
    else {
        return "";
    }
}

string Parser::dest() {
    if (instructionType() == Type::C_INSTRUCTION) {
        auto findResult = std::find(curInstruction.begin(), curInstruction.end(), '=');
        if (findResult != curInstruction.end()) {
            return curInstruction.substr(0, findResult-curInstruction.begin());
        }
    }
    return "";
}

string Parser::comp() {
    if (instructionType() == Type::C_INSTRUCTION) {
        auto find1 = std::find(curInstruction.begin(), curInstruction.end(), '=');
        auto find2 = std::find(curInstruction.begin(), curInstruction.end(), ';');
        if (find1 != curInstruction.end()) {
            return curInstruction.substr(find1-curInstruction.begin()+1, find2-find1-1);
        }
        else {
            return curInstruction.substr(0, find2-curInstruction.begin());
        }
    }
    return "";
}

string Parser::jump() {
    if (instructionType() == Type::C_INSTRUCTION) {
        auto findResult = std::find(curInstruction.begin(), curInstruction.end(), ';');
        if (findResult != curInstruction.end()) {
            return curInstruction.substr(findResult-curInstruction.begin()+1);
        }
    }
    return "";
}

string Parser::curIns() {
    return curInstruction;
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

void Parser::reset() {
    source.clear();
    source.seekg(0, std::ios_base::beg);
    curInstruction="";
}