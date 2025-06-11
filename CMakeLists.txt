cmake_minimum_required(VERSION 3.10)
project(CustomOptional)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(optional src/main.cpp)
