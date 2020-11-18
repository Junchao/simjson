cmake_minimum_required(VERSION 3.10)
project(Unity C)
set(CMAKE_C_STANDARD 99)

add_library(Unity STATIC src/unity.c extras/fixture/src/unity_fixture.c)
target_include_directories(Unity PUBLIC src extras/fixture/src)