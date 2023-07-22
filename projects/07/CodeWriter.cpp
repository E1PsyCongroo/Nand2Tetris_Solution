/*Generates assembly code from the parsed VM command*/
#pragma once

#include "CodeWriter.hpp"
#include "Parser.hpp"
#include <stdexcept>
using std::string;
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

void CodeWriter::writerPushD() {
    ofs << "@SP\n"
        << "A=M\n"
        << "M=D\n"
        << "@SP\n"
        << "M=M+1\n";
}

void CodeWriter::writerPopD() {
    ofs << "@SP\n"
        << "AM=M-1\n"
        << "D=M\n";
}

void CodeWriter::writerPush(string segment, int index) {
    ofs << "// push " << segment << " " << index << '\n';
    if (segment == "constant") {
        ofs << "@" << index << '\n'
            << "D=A\n";
    }
    else if (segment == "static") {
        ofs << "@" << filename << "." << index << '\n'
            << "D=M\n";
    }
    else if (segment == "pointer") {
        if (index == 0) {
            ofs << "@THIS\n"
                << "D=M\n";
        }
        else if (index == 1) {
            ofs << "@THAT\n"
                << "D=M\n";
        }
        else {
            throw std::runtime_error("push pointer not 0/1!");
        }
    }
    else if (segment == "temp") {
        ofs << "@" << 5+index << '\n'
            << "D=M\n";
    }
    else {
        ofs << "@" << index << '\n'
            << "D=A\n"
            << "@" << segmentMap[segment] << '\n'
            << "A=M+D\n"
            << "D=M\n";
    }
    writerPushD();
}

void CodeWriter::writerPop(string segment, int index) {
    ofs << "// pop " << segment << " " << index << '\n';
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
    ofs << "// " << command << '\n';
    if (command == "add") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=M+D\n";
    }
    else if (command == "sub") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=M-D\n";
    }
    else if (command == "neg") {
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=-M\n";
    }
    else if (command == "eq") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "D=M-D\n"
            << "M=-1\n"
            << '@' << command << '_' << symbolIndex << '\n'
            << "D;JEQ\n"
            << "@SP\n"
            << "A=M\n"
            << "M=0\n"
            << '(' << command << '_' << symbolIndex++ << ")\n";
    }
    else if (command == "gt") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "D=M-D\n"
            << "M=-1\n"
            << '@' << command << '_' << symbolIndex << '\n'
            << "D;JGT\n"
            << "@SP\n"
            << "A=M\n"
            << "M=0\n"
            << '(' << command << '_' << symbolIndex++ << ")\n";
    }
    else if (command == "lt") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "D=M-D\n"
            << "M=-1\n"
            << '@' << command << '_' << symbolIndex << '\n'
            << "D;JLT\n"
            << "@SP\n"
            << "A=M\n"
            << "M=0\n"
            << '(' << command << '_' << symbolIndex++ << ")\n";
    }
    else if (command == "and") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=M&D\n";
    }
    else if (command == "or") {
        writerPopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=M|D\n";
    }
    else if (command == "not") {
        ofs << "@SP\n"
            << "AM=M-1\n"
            << "M=!M\n";
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
    ofs << "(END)\n"
        << "@END\n"
        << "0;JMP\n";
}