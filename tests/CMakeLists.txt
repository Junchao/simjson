cmake_minimum_required(VERSION 3.10)
project(tests C)
set(CMAKE_C_STANDARD 99)

add_subdirectory(Unity)

file(GLOB TEST_FILES test_*.c)
foreach (test_file ${TEST_FILES})
    get_filename_component(test_file_name ${test_file} NAME)
    string(REPLACE ".c" "" test_name ${test_file_name})
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} Unity Simjson)
endforeach (test_file ${TEST_FILES})