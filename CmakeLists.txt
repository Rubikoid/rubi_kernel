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

add_executable(kernel "kernel/kernel.c" "loader/start.asm")
target_compile_options(kernel PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu99> $<$<COMPILE_LANGUAGE:C>:-ffreestanding> -g)
set(CMAKE_ASM_NASM_FLAGS_DEBUG "-Fdwarf")
target_link_options(kernel PRIVATE -ffreestanding -nostdlib -g -lgcc -T ${CMAKE_SOURCE_DIR}/loader/linker.ld)

add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/iso/boot/$<TARGET_FILE_NAME:kernel>)
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/grub ${CMAKE_BINARY_DIR}/iso/boot/grub)
add_custom_command(TARGET kernel POST_BUILD COMMAND genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A ${PROJECT_NAME} -input-charset utf8 -quiet -boot-info-table -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso ${CMAKE_BINARY_DIR}/iso)

# generate debug info for bochs
add_custom_command(TARGET kernel POST_BUILD COMMAND objcopy --only-keep-debug $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/kernel.sym.bin)
add_custom_command(TARGET kernel POST_BUILD COMMAND bash -c "nm ${CMAKE_BINARY_DIR}/kernel.sym.bin | grep \" [Tbt] \" | awk '{ print \$1\" \"\$3 }' > ${CMAKE_BINARY_DIR}/kernel.sym " VERBATIM)
