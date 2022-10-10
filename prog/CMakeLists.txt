CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

add_subdirectory(shell_d)
# add_subdirectory(shell)

add_custom_target(programms)
add_dependencies(programms shell_d)
