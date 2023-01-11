CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/toolchains/gcc.toolchain.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/dlang)

project(LibD C D)

file(GLOB_RECURSE LIBD_SRC ${PROJECT_SOURCE_DIR}/libd/*.d)

add_library(d STATIC ${LIBD_SRC})

set_target_properties(d
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_options(
    d
    PRIVATE
    -g
    -I ${PROJECT_SOURCE_DIR}/prog/shell_d/
    )
target_compile_definitions(d PRIVATE "_DEBUG=1")
