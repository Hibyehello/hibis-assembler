cmake_minimum_required(VERSION 3.15)

project(HIBIS)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Command-Line-Parser)

add_executable(hibis-assembler
  main.cpp
  Assembler.cpp
  Assembler.h
)

target_link_libraries(hibis-assembler HIB_Parser)
