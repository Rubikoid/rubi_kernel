INCLUDE(CMakeForceCompiler)

# SET(CMAKE_SYSTEM_NAME Generic)
# SET(CMAKE_SYSTEM_VERSION 1)

set(CLANG_TARGET_TRIPLE "i686-pc-none-elf")

set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(ARCH_FLAGS "-march=i686")
set(CMAKE_C_FLAGS "-Wall -std=gnu99 ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
set(CMAKE_CXX_FLAGS "-Wall -fno-pie -std=c++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics ${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
set(CMAKE_D_FLAGS "--mtriple=${CLANG_TARGET_TRIPLE} --betterC --disable-red-zone --fno-plt --nogc --relocation-model=static" CACHE STRING "Common flags for D compiler")
set(CMAKE_EXE_LINKER_FLAGS "") # -flto -static -fno-pie -m32 -ffreestanding -nostdlib


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
