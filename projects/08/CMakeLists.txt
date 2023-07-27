cmake_minimum_required(VERSION 3.27)

project(VMTranslator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(CodeWriter CodeWriter.cpp)
add_library(Parser Parser.cpp)

add_executable(VMTranslator VMTranslator.cpp)

target_link_libraries(VMTranslator PUBLIC CodeWriter Parser)

target_include_directories(VMTranslator PUBLIC
                            "${PROJECT_SOURCE_DIR}"
                            "${PROJECT_BINARY_DIR}")