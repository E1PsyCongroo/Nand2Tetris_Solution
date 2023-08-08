cmake_minimum_required(VERSION 3.27)

project(JackCompiler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(JackTokenizer JackTokenizer.cpp)
add_library(SymbolTable SymbolTable.cpp)
add_library(VMWriter VMWriter.cpp)
add_library(CompilationEngine CompilationEngine.cpp JackTokenizer.cpp SymbolTable.cpp VMWriter.cpp)

add_executable(JackCompiler JackCompiler.cpp)

target_link_libraries(JackCompiler PUBLIC JackTokenizer SymbolTable VMWriter CompilationEngine)

target_include_directories(JackCompiler PUBLIC
                            "${PROJECT_SOURCE_DIR}"
                            "${PROJECT_BINARY_DIR}")