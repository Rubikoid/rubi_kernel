CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

file(GLOB_RECURSE PROG_SRC ${PROJECT_SOURCE_DIR}/prog/shell/shell/src/*.c)
file(GLOB_RECURSE PROG_ASM_SRC ${PROJECT_SOURCE_DIR}/prog/shell/shell/src/*.asm)
file(GLOB_RECURSE PROG_HEAD ${PROJECT_SOURCE_DIR}/prog/shell/shell/include/*.h)

add_executable(shell ${PROG_ASM_SRC} ${PROG_SRC} ${PROG_HEAD} ${KERNEL_HEAD})

set_target_properties(shell
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/initrd/bin"
)

target_include_directories(shell PRIVATE "shell/include") # add includes
target_include_directories(shell PRIVATE "${CMAKE_SOURCE_DIR}/kernel/include") # add includes

target_compile_options(shell PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu99> $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector> $<$<COMPILE_LANGUAGE:C>:-nostdinc> $<$<COMPILE_LANGUAGE:C>:-ffreestanding> $<$<COMPILE_LANGUAGE:C>:-m32> -g -Wall) # compile without linux gcc shit
target_compile_definitions(shell PRIVATE "_DEBUG=1")
target_link_options(shell PRIVATE -m32 -ffreestanding -nostdlib -g -lgcc -T ${CMAKE_SOURCE_DIR}/prog/linker.ld) #link everything using linkerscript # 
target_link_libraries(shell libc_library)
