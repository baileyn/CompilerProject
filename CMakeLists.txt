cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(CompilerProject)

set(SOURCE_FILES src/main.cpp
    src/Tokenizer.cpp src/Tokenizer.hpp
    src/Parser.cpp src/Parser.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
