CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

file(GLOB_RECURSE PROG_SRC ${PROJECT_SOURCE_DIR}/prog/shell_d/*.d)

add_executable(shell_d ${PROG_SRC})

set_target_properties(shell_d
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/bin"
)

target_compile_options(
    shell_d
    PRIVATE
    -g
    -I ${PROJECT_SOURCE_DIR}/prog/shell_d/
    )
target_compile_definitions(shell_d PRIVATE "_DEBUG=1")

target_link_libraries(shell_d d)

target_link_options(shell_d PRIVATE -g -T ${CMAKE_SOURCE_DIR}/prog/linker.ld) #link everything using linkerscript # 
