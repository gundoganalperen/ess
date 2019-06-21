cmake_minimum_required(VERSION 3.9)
project(PartA)

set(CMAKE_CXX_STANDARD 11)

add_executable(PartA
        Descriptors.c
        Descriptors.h
        german_keyboardCodes.h
        KeyboardHID.c
        KeyboardHID.h
        main.c)
