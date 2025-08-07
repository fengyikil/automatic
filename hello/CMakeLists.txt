cmake_minimum_required(VERSION 3.20.0)
project(my_hello)
set(CMAKE_CXX_STANDARD 11)

add_compile_options(-finput-charset=UTF-8)
add_compile_options(-fexec-charset=UTF-8)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(
    my_hello
    ${SRC_LIST}
)
