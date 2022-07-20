INCLUDE(CMakeForceCompiler)

# SET(CMAKE_SYSTEM_NAME Generic)
# SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_SYSTEM_PROCESSOR i686)

set(BUILD_PREFIX "$ENV{HOME}/opt/cross")

set(CMAKE_D_COMPILER "${BUILD_PREFIX}/bin/i686-rkernel-gdc")
set(CMAKE_C_COMPILER "${BUILD_PREFIX}/bin/i686-rkernel-gcc")
set(CMAKE_ASM_COMPILER "${BUILD_PREFIX}/bin/i686-rkernel-gcc")
set(CMAKE_CXX_COMPILER "${BUILD_PREFIX}/bin/i686-rkernel-g++")

set(ARCH_FLAGS "-m32")

set(CMAKE_C_FLAGS "-Wall -std=gnu99 ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
set(CMAKE_CXX_FLAGS "-Wall ${ARCH_FLAGS} -fno-pie -std=c++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics" CACHE STRING "Common flags for C++ compiler")
# -nophoboslib -fno-exceptions -fno-moduleinfo -fno-rtti == 
set(CMAKE_D_FLAGS "-Wall ${ARCH_FLAGS} -fno-exceptions -fno-moduleinfo -fno-rtti" CACHE STRING "Common flags for D compiler")
set(CMAKE_EXE_LINKER_FLAGS "") # -flto -static -fno-pie -m32 -ffreestanding -nostdlib

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
