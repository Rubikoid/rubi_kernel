CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(rubiKernel C CXX ASM_NASM)
# enable_language(ASM-ATT)

# set(PREFIX "$ENV{HOME}/gcc_cross/opt")
# set(CMAKE_C_COMPILER "${PREFIX}/bin/i686-elf-gcc")
# set(CMAKE_ASM_COMPILER "${PREFIX}/bin/i686-elf-gcc")
# set(CMAKE_CXX_COMPILER "${PREFIX}/bin/i686-elf-g++")

# set(CMAKE_NASM_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/iso" "${CMAKE_BINARY_DIR}/iso/boot" "${CMAKE_BINARY_DIR}/iso/boot/grub")
file(GLOB_RECURSE KERNEL_SRC ${PROJECT_SOURCE_DIR}/kernel/src/*.c)
file(GLOB_RECURSE KERNEL_ASM_SRC ${PROJECT_SOURCE_DIR}/kernel/src/*.asm)
file(GLOB_RECURSE KERNEL_LOADER_ASM_SRC ${PROJECT_SOURCE_DIR}/loader/*.asm)
file(GLOB_RECURSE KERNEL_HEAD ${PROJECT_SOURCE_DIR}/kernel/include/*.h)

file(GLOB_RECURSE KERNEL_LIB_SRC ${PROJECT_SOURCE_DIR}/kernel/src/lib/*.c)
file(GLOB_RECURSE KERNEL_LIB_HEAD ${PROJECT_SOURCE_DIR}/kernel/include/lib/*.h)
file(GLOB_RECURSE KERNEL_LIB_ASM_SRC ${PROJECT_SOURCE_DIR}/kernel/src/lib/*.asm)

add_executable(kernel ${KERNEL_LOADER_ASM_SRC} ${KERNEL_ASM_SRC} ${KERNEL_SRC} ${KERNEL_HEAD})

target_include_directories(kernel PRIVATE "kernel/include") # add includes

set(CMAKE_ASM_NASM_FLAGS_DEBUG "-Fdwarf -32") # some debug info, i hope

target_compile_options(kernel PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu99> $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector> $<$<COMPILE_LANGUAGE:C>:-nostdinc> $<$<COMPILE_LANGUAGE:C>:-ffreestanding> $<$<COMPILE_LANGUAGE:C>:-m32> -g -Wall) # compile without linux gcc shit
target_compile_definitions(kernel PRIVATE "KERNEL=1" "_DEBUG=1" "KERNEL_VERSION=\"v0.0.18\"")
target_link_options(kernel PRIVATE -m32 -ffreestanding -nostdlib -g -lgcc -T ${CMAKE_SOURCE_DIR}/loader/linker.ld) #link everything using linkerscript

#include(prog/CMakeLists.txt)
add_subdirectory(prog)

# generate iso file for bochs
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/iso/boot/$<TARGET_FILE_NAME:kernel>)
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/grub ${CMAKE_BINARY_DIR}/iso/boot/grub)
add_custom_command(TARGET kernel POST_BUILD COMMAND genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A ${PROJECT_NAME} -input-charset utf8 -quiet -boot-info-table -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso ${CMAKE_BINARY_DIR}/iso)
# add_custom_command(TARGET kernel POST_BUILD COMMAND grub-mkrescue -o  ${CMAKE_BINARY_DIR}/grub.iso  ${CMAKE_BINARY_DIR}/iso)
# add_custom_command(TARGET kernel POST_BUILD COMMAND xz -zfkv -T 0 ${CMAKE_BINARY_DIR}/grub.iso)


# generate debug info for bochs
add_custom_command(TARGET kernel POST_BUILD COMMAND objcopy --only-keep-debug $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/kernel.sym.bin)
add_custom_command(TARGET kernel POST_BUILD COMMAND bash -c "nm ${CMAKE_BINARY_DIR}/kernel.sym.bin | grep \" [Tbt] \" | awk '{ print \$1\" \"\$3 }' > ${CMAKE_BINARY_DIR}/kernel.sym " VERBATIM)

add_custom_target(run   COMMAND qemu-system-x86_64          -m 64                       -kernel ./kernel -initrd ./prog/shell/prog DEPENDS prog kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(rung  COMMAND qemu-system-x86_64 -s       -m 64                       -kernel ./kernel -initrd ./prog/shell/prog DEPENDS prog kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(rungs COMMAND qemu-system-x86_64 -s -S    -m 64                       -kernel ./kernel -initrd ./prog/shell/prog DEPENDS prog kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(runc  COMMAND qemu-system-x86_64          -m 64 -nographic            -kernel ./kernel -initrd ./prog/shell/prog DEPENDS prog kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(runcr COMMAND qemu-system-x86_64          -m 64 -nographic -no-reboot -kernel ./kernel -initrd ./prog/shell/prog DEPENDS prog kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
