/*Generates assembly code from the parsed VM command*/
#pragma once

#include "CodeWriter.hpp"
#include "Parser.hpp"
#include <stdexcept>
#include <set>
using std::string; using std::set;
CodeWriter::CodeWriter(string filename): ofs{filename} {
    auto first = filename.find_last_of('\\');
    auto last = filename.find_last_of('.');
    if (first != std::string::npos) {
        this->filename = filename.substr(first+1, last-first-1);
    }
    else {
        this->filename = filename.substr(0, last-1);
    }
    symbolIndex = 0;
}
CodeWriter::~CodeWriter() {
    ofs.close();
}

void CodeWriter::writeComment(string command) {
    ofs << "// " << command << '\n';
}

// push(D)
void CodeWriter::writerPushD() {
    ofs << "@SP\n"
        << "A=M\n"
        << "M=D\n"
        << "@SP\n"
        << "M=M+1\n";
}

// D = pop()
void CodeWriter::writerPopD() {
    ofs << "@SP\n"
        << "AM=M-1\n"
        << "D=M\n";
}

// push segment index
void CodeWriter::writerPush(string segment, int index) {
    if (segment == "constant") {
        // D = index
        ofs << "@" << index << '\n'
            << "D=A\n";
    }
    else if (segment == "static") {
        // D = STATIC[index]
        ofs << "@" << filename << "." << index << '\n'
            << "D=M\n";
    }
    else if (segment == "pointer") {
        if (index == 0) {
            // D = *THIS
            ofs << "@THIS\n"
                << "D=M\n";
        }
        else if (index == 1) {
            // D = *THAT
            ofs << "@THAT\n"
                << "D=M\n";
        }
        else {
            throw std::runtime_error("push pointer not 0/1!");
        }
    }
    else if (segment == "temp") {
        // D = TEMP[index]
        ofs << "@" << 5+index << '\n'
            << "D=M\n";
    }
    else {
        // D = LCL/THIS/THAT/ARGUMENT[index]
        ofs << "@" << index << '\n'
            << "D=A\n"
            << "@" << segmentMap[segment] << '\n'
            << "A=M+D\n"
            << "D=M\n";
    }
    writerPushD();
}

void CodeWriter::writerPop(string segment, int index) {
    if (segment == "constant") {
        throw std::runtime_error("can't pop constant!");
    }
    else if (segment == "static") {
        writerPopD();
        ofs << "@" << filename << "." << index << '\n'
            << "M=D\n";
    }
    else if (segment == "pointer") {
        writerPopD();
        if (index == 0) {
            ofs << "@THIS\n"
                << "M=D\n";
        }
        else if (index == 1) {
            ofs << "@THAT\n"
                << "M=D\n";
        }
        else {
            throw std::runtime_error("push pointer not 0/1!");
        }
    }
    else if (segment == "temp") {
        writerPopD();
        ofs << "@" << 5+index << '\n'
            << "M=D\n";
    }
    else {
        ofs << "@" << index << '\n'
            << "D=A\n"
            << "@" << segmentMap[segment] << '\n'
            << "D=M+D\n"
            << "@R13\n"
            << "M=D\n";
        writerPopD();
        ofs << "@R13\n"
            << "A=M\n"
            << "M=D\n";
    }
}

void CodeWriter::writeArithmetic(string command) {
    const set<string> binary = {"add", "sub", "and", "or"};
    const set<string> predicate = {"neg", "not"};
    const set<string> logical = {"eq", "gt", "lt"};
    if (binary.count(command)) {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << arithmeticMap[command];
    }
    else if (predicate.count(command)) {
        ofs << "@SP\n"
            << "AM=M-1\n"
            << arithmeticMap[command];
    }
    else if (logical.count(command)) {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "D=M-D\n"
            << "M=-1\n"
            << "@__" << command << '_' << symbolIndex << '\n'
            << arithmeticMap[command]
            << "@SP\n"
            << "A=M\n"
            << "M=0\n"
            << "(__" << command << '_' << symbolIndex++ << ")\n";
    }
    else {
        throw std::runtime_error("unknow AL command!");
    }
    ofs << "@SP\n"
        << "M=M+1\n";
}

void CodeWriter::writerPushPop(Parser::CType commandType, string segment, int index) {
    if (commandType == Parser::CType::C_PUSH) {
        writerPush(segment, index);
    }
    else if (commandType == Parser::CType::C_POP) {
        writerPop(segment, index);
    }
    else {
        throw std::runtime_error("in writerPushPop commandType Error!");
    }
}

void CodeWriter::writerEnd() {
    ofs << "(__END)\n"
        << "@__END\n"
        << "0;JMP\n";
}