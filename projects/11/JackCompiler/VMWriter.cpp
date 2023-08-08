#include "VMWriter.hpp"
#include <stdexcept>

VMWriter::VMWriter(string filename): ofs{filename} {
    if (!ofs.is_open()) {
        throw std::runtime_error(filename + " open fail");
    }
}

VMWriter::~VMWriter() {
    ofs.close();
}

void VMWriter::writePush(string segment, int index) {
    ofs << "push " << segment << " " << index << '\n';
}

void VMWriter::writePop(string segment, int index) {
    ofs << "pop " << segment << " " << index << '\n';
}

void VMWriter::writeArithmetic(string command) {
    ofs << command << '\n';
}

void VMWriter::writeLabel(string label) {
    ofs << "label " << label << '\n';
}

void VMWriter::writeGoto(string label) {
    ofs << "goto " << label << '\n';
}

void VMWriter::writeIf(string label) {
    ofs << "if-goto " << label << '\n';
}

void VMWriter::writeCall(string name, int nArgs) {
    ofs << "call " << name << ' ' << nArgs << '\n';
}

void VMWriter::writeFunction(string name, int nVars) {
    ofs << "function " << name << ' ' << nVars << '\n';
}

void VMWriter::writeReturn() {
    ofs << "return\n";
}

void VMWriter::setFile(string filename) {
    ofs.close();
    ofs.open(filename);
    if (!ofs.is_open()) {
        throw std::runtime_error(filename + " open fail");
    }
}
