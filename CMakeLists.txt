cmake_minimum_required(VERSION 3.10)

# set the project name
project(HW1)

# add the executable
add_executable(HW1 commands.c commands.h signals.c signals.h smash.c)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)