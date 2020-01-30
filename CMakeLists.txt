cmake_minimum_required(VERSION 3.15)
project(SmartPtr C)

set(CMAKE_C_STANDARD 11)

add_executable(SmartPtr main.c smart-ptr.c smart-ptr.h)