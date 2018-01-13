cmake_minimum_required(VERSION 3.9)
project(CS143B C)

set(CMAKE_C_STANDARD 99)

add_executable(CS143B main.c input.txt macros.h output.txt
        shell_commands.c random.c structures.h)