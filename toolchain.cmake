INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

set(GCC_TOOLCHAIN "/Users/rubikoid/opt/cross")
LIST(APPEND CMAKE_PROGRAM_PATH ${GCC_TOOLCHAIN})

set(CLANG_TARGET_TRIPLE "i686-pc-none-elf")

set(BUILD_PREFIX "/opt/homebrew/opt/llvm")

set(CMAKE_SYSROOT ${BUILD_PREFIX})
set(CMAKE_FIND_ROOT_PATH ${GCC_SYSROOT})

set(CMAKE_C_COMPILER "${BUILD_PREFIX}/bin/clang")
set(CMAKE_C_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

set(CMAKE_ASM_COMPILER "${BUILD_PREFIX}/bin/clang")
set(CMAKE_ASM_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

set(CMAKE_CXX_COMPILER "${BUILD_PREFIX}/bin/clang++")
set(CMAKE_CXX_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

# set(LD_LLD "/Users/rubikoid/opt/cross/bin/i686-elf-ld") # "${BUILD_PREFIX}/bin/ld.lld")
# set(CMAKE_LIBRARY_ARCHITECTURE ${CLANG_TARGET_TRIPPLE})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(ARCH_FLAGS "-march=i686")
set(CMAKE_C_FLAGS "-Wall ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
set(CMAKE_CXX_FLAGS "-Wall -std=c++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics ${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
set(CMAKE_ASM_FLAGS "-Wall ${ARCH_FLAGS}" CACHE STRING "Common flags for assembler")

# link_libraries(-Wl,--build-id -fuse-ld=${LD_LLD})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "")
# set(CMAKE_MODULE_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "")
# set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "")

# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
# set(CMAKE_ASM_COMPILER_WORKS 1)

# CMAKE_FORCE_C_COMPILER(arm-elf-gcc GNU)
# CMAKE_FORCE_CXX_COMPILER(arm-elf-g++ GNU)

# CMAKE_FORCE_C_COMPILER("${BUILD_PREFIX}/bin/i686-elf-gcc" GNU)
# CMAKE_FORCE_ASM_COMPILER("${BUILD_PREFIX}/bin/i686-elf-gcc" GNU)
# CMAKE_FORCE_CXX_COMPILER("${BUILD_PREFIX}/bin/i686-elf-g++" GNU)


SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

