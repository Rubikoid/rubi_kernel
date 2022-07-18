CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

file(GLOB_RECURSE PROG_SRC ${PROJECT_SOURCE_DIR}/prog/shell.d/shell.d/src/*.d)
file(GLOB_RECURSE PROG_ASM_SRC ${PROJECT_SOURCE_DIR}/prog/shell.d/shell.d/src/*.asm)
file(GLOB_RECURSE PROG_HEAD ${PROJECT_SOURCE_DIR}/prog/shell.d/shell.d/include/*.h)

add_executable(shell_d ${PROG_ASM_SRC} ${PROG_SRC} ${PROG_HEAD} ${KERNEL_HEAD})

set_target_properties(shell_d
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/bin"
)

target_include_directories(shell_d PRIVATE "shell/include") # add includes
target_include_directories(shell_d PRIVATE "${CMAKE_SOURCE_DIR}/kernel/include") # add includes

target_compile_options(
    shell_d
    PRIVATE
    -g)
target_compile_definitions(shell_d PRIVATE "_DEBUG=1")
target_link_options(shell_d PRIVATE -g) #link everything using linkerscript # -T ${CMAKE_SOURCE_DIR}/prog/linker.ld
target_link_libraries(shell_d libc_library)
