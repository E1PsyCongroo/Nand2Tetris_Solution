cmake_minimum_required(VERSION 3.27)

project(JackAnalyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(JackTokenizer JackTokenizer.cpp)
add_library(CompilationEngine CompilationEngine.cpp JackTokenizer.cpp)

add_executable(JackAnalyzer JackAnalyzer.cpp)

target_link_libraries(JackAnalyzer PUBLIC JackTokenizer CompilationEngine)

target_include_directories(JackAnalyzer PUBLIC
                            "${PROJECT_SOURCE_DIR}"
                            "${PROJECT_BINARY_DIR}")