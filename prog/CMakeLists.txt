CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

add_subdirectory(rlibc)
add_subdirectory(shell)

add_custom_target(programms)
add_dependencies(programms libc_library shell)
