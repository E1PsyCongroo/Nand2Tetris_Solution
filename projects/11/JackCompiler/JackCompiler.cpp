#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
using std::cout; using std::endl;
using std::string; using std::ofstream;
using std::ifstream;

using namespace std::filesystem;

int main(const int argc, const char* argv[]) {

    if (argc != 2 && argc != 3) {
        cout << "Usage: JackAnalyzer source(regularFile/dir)\n";
        exit(1);
    }
    path inputPath = argv[1];
    if (is_regular_file(inputPath)) {
        string input = inputPath.string();
        string output = inputPath.parent_path().string() + '/' + inputPath.stem().string();
        cout << "Input PATH: " << input << '\n';
        cout << "Output PATH: " << output << '\n';
        CompilationEngine compilationEngine(input, output+".vm");
        compilationEngine.compileClass();
    }
    else {
        for (auto& dir_entry : directory_iterator{inputPath}) {
            if (dir_entry.is_regular_file() && dir_entry.path().extension().string() == ".jack") {
                string input = dir_entry.path().string();
                string output = dir_entry.path().parent_path().string() + '/' + dir_entry.path().stem().string();
                cout << "Input PATH: " << input << '\n';
                cout << "Output PATH: " << output << '\n';
                CompilationEngine compilationEngine(input, output+".vm");
                compilationEngine.compileClass();
            }
        }
    }
    return 0;
}