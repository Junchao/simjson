cmake_minimum_required(VERSION 3.10)
project(Simjson C)
set(CMAKE_C_STANDARD 99)

file(GLOB SIMJSON_SRC src/*.c)
add_library(Simjson SHARED ${SIMJSON_SRC})
target_include_directories(Simjson PUBLIC include)

add_subdirectory(examples)

add_subdirectory(tests)
enable_testing()

add_test(
        NAME test_simjson_string
        COMMAND test_simjson_string
)
add_test(
        NAME test_simjson_number
        COMMAND test_simjson_number
)
add_test(
        NAME test_simjson_boolean
        COMMAND test_simjson_boolean
)
add_test(
        NAME test_simjson_null
        COMMAND test_simjson_null
)
add_test(
        NAME test_simjson_array
        COMMAND test_simjson_array
)
add_test(
        NAME test_simjson_object
        COMMAND test_simjson_object
)
add_test(
        NAME test_simjson_decode_encode
        COMMAND test_simjson_decode_encode
)