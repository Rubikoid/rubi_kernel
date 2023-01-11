CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/toolchains/gcc.toolchain.cmake)

project(LibR C ASM_NASM)

# === Init NASM assembler ===
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)

file(GLOB_RECURSE LIBR_SRC ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE LIBR_ASM_SRC ${PROJECT_SOURCE_DIR}/src/*.asm)

add_library(r STATIC ${LIBR_ASM_SRC} ${LIBR_SRC})

set_target_properties(r
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(r PUBLIC "${PROJECT_SOURCE_DIR}/include") # add includes
target_include_directories(r PRIVATE "${PROJECT_SOURCE_DIR}/../../kernel/include") # add includes

target_compile_options(
    r
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
    $<$<COMPILE_LANGUAGE:C>:-nostdinc>
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    -g -Wall) # compile without linux gcc shit
target_compile_definitions(r PRIVATE "_DEBUG=1")
