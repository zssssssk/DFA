cmake_minimum_required(VERSION 3.19)
project(DFA)

set(CMAKE_CXX_STANDARD 17)

add_executable(DFA main.cpp suffixForm.h NFAForm.h DFAForm.h)