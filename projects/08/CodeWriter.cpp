/*Generates assembly code from the parsed VM command*/
#include "CodeWriter.hpp"
#include "Parser.hpp"
#include <stdexcept>
#include <set>
using std::string; using std::set;
CodeWriter::CodeWriter(string openfile, string filename): ofs{openfile}, filename{filename} {
    symbolIndex = 0;
    returnIndex = 0;
}
CodeWriter::~CodeWriter() {
    ofs.close();
}

void CodeWriter::writeComment(string command) {
    ofs << "// " << command << '\n';
}

// push(D)
void CodeWriter::writePushD() {
    ofs << "@SP\n"
        << "A=M\n"
        << "M=D\n"
        << "@SP\n"
        << "M=M+1\n";
}

// D = pop()
void CodeWriter::writePopD() {
    ofs << "@SP\n"
        << "AM=M-1\n"
        << "D=M\n";
}

// push segment index
void CodeWriter::writePush(string segment, int index) {
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
    writePushD();
}

void CodeWriter::writePop(string segment, int index) {
    if (segment == "constant") {
        throw std::runtime_error("can't pop constant!");
    }
    else if (segment == "static") {
        // static[index] = pop()
        writePopD();
        ofs << "@" << filename << "." << index << '\n'
            << "M=D\n";
    }
    else if (segment == "pointer") {
        // pointer[index] = pop()
        writePopD();
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
        // temp[index] = pop()
        writePopD();
        ofs << "@" << 5+index << '\n'
            << "M=D\n";
    }
    else {
        // LCL/THIS/THAT/ARGUMENT[index] = pop()
        ofs << "@" << index << '\n'
            << "D=A\n"
            << "@" << segmentMap[segment] << '\n'
            << "D=M+D\n"
            << "@R13\n"
            << "M=D\n";
        writePopD();
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
        // push(pop() <binaryOP> pop())
        writePopD();
        ofs << "@SP\n"
            << "AM=M-1\n"
            << arithmeticMap[command];
    }
    else if (predicate.count(command)) {
        // push(<predicateOP> pop())
        ofs << "@SP\n"
            << "AM=M-1\n"
            << arithmeticMap[command];
    }
    else if (logical.count(command)) {
        // push(pop() <logicalOP> pop())
        writePopD();
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

void CodeWriter::writePushPop(Parser::CType commandType, string segment, int index) {
    if (commandType == Parser::CType::C_PUSH) {
        writePush(segment, index);
    }
    else if (commandType == Parser::CType::C_POP) {
        writePop(segment, index);
    }
    else {
        throw std::runtime_error("in writerPushPop commandType Error!");
    }
}

void CodeWriter::writeLabel(string label) {
    ofs << '(' << label << ")\n";
}

void CodeWriter::writeGoto(string label) {
    ofs << '@' << label << '\n'
        << "0;JMP\n";
}

void CodeWriter::writeIf(string label) {
    writePopD();
    ofs << '@' << label << '\n'
        << "D;JNE\n";
}

void CodeWriter::writeFunction(string functionName, int nVars) {
    string loopLabel = "__LOOP_" + std::to_string(symbolIndex++);
    string endLoopLabel = "__ENDLOOP_" + std::to_string(symbolIndex++);
    curFunctionName = functionName;
    returnIndex = 0;
    ofs << '(' << functionName << ")\n" // (functionName)
        << '@' << nVars << '\n'         // R13=nVars
        << "D=A\n"
        << "@R13\n"
        << "M=D\n";
    writeLabel(loopLabel);  // (loopLabel)
    ofs << "@R13\n" // if R13=0 goto endLoopLabel
        << "D=M\n"
        << "@" << endLoopLabel << '\n'
        << "D;JEQ\n";
    writePush("constant", 0); // push 0
    ofs << "@R13\n"    // R13=R13-1
        << "M=M-1\n";
    writeGoto(loopLabel);   // goto loopLabel
    writeLabel(endLoopLabel);
}

void CodeWriter::writeCall(string functionName, int nArgs) {
    string returnAddress = curFunctionName + "$ret." + std::to_string(returnIndex++);
    ofs << '@' << returnAddress << '\n' // push returnAddress
        << "D=A\n";
    writePushD();
    ofs << "@LCL\n" // push LCL
        << "D=M\n";
    writePushD();
    ofs << "@ARG\n" // push ARG
        << "D=M\n";
    writePushD();
    ofs << "@THIS\n"// push THIS
        << "D=M\n";
    writePushD();
    ofs << "@THAT\n"// push THAT
        << "D=M\n";
    writePushD();
    ofs << "@" << 5+nArgs << '\n'   // R13 = 5+nArgs
        << "D=A\n"
        << "@R13\n"
        << "M=D\n"
        << "@SP\n"  // D = SP-R13
        << "D=M\n"
        << "@R13\n"
        << "D=D-M\n"
        << "@ARG\n" // ARG = D = SP-R13 = SP - 5 - nArgs
        << "M=D\n"
        << "@SP\n"  // D = SP
        << "D=M\n"
        << "@LCL\n" // LCL = D = SP
        << "M=D\n";
    writeGoto(functionName);
    writeLabel(returnAddress);
}

void CodeWriter::writeReturn() {
    ofs << "@LCL\n" // frame = R13 = LCL
        << "D=M\n"
        << "@R13\n"
        << "M=D\n"
        << "@5\n"   // retAddr = R14 = *(LCL-5)
        << "A=D-A\n"
        << "D=M\n"
        << "@R14\n"
        << "M=D\n";
    writePopD();    // *ARG = pop()
    ofs << "@ARG\n"
        << "A=M\n"
        << "M=D\n"
        << "@ARG\n" // SP = ARG+1
        << "D=M\n"
        << "@SP\n"
        << "M=D+1\n"
        << "@R13\n" // THAT = *(--frame)
        << "AM=M-1\n"
        << "D=M\n"
        << "@THAT\n"
        << "M=D\n"
        << "@R13\n" // THIS = *(--frame)
        << "AM=M-1\n"
        << "D=M\n"
        << "@THIS\n"
        << "M=D\n"
        << "@R13\n" // ARG = *(--frame)
        << "AM=M-1\n"
        << "D=M\n"
        << "@ARG\n"
        << "M=D\n"
        << "@R13\n" // LCL = *(--frame)
        << "AM=M-1\n"
        << "D=M\n"
        << "@LCL\n"
        << "M=D\n"
        << "@R14\n" // goto retAddr
        << "A=M\n"
        << "0;JMP\n";
}

void CodeWriter::writeInit() {
    ofs << "@256\n" // SP = 256
        << "D=A\n"
        << "@SP\n"
        << "M=D\n";
    writeCall("Sys.init", 0);   // call Sys.init
}

void CodeWriter::writeEnd() {
    ofs << "(__END)\n"
        << "@__END\n"
        << "0;JMP\n";
}

void CodeWriter::setFilename(string filename) {
    this->filename = filename;
}