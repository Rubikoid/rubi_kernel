CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

# === Select crosscompiler toolchain ===
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/llvm.toolchain.cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils/toolchains/gcc.toolchain.cmake)

# === Setup D lang ===
# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dlang)

# === Init kernel ===
project(rubiKernel C CXX ASM_NASM)

# set(CMAKE_NASM_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# === Init NASM assembler ===
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)

# === Collect source folders ===
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/iso" "${CMAKE_BINARY_DIR}/iso/boot" "${CMAKE_BINARY_DIR}/iso/boot/grub")
file(GLOB_RECURSE KERNEL_SRC ${PROJECT_SOURCE_DIR}/kernel/src/*.c)
file(GLOB_RECURSE KERNEL_ASM_SRC ${PROJECT_SOURCE_DIR}/kernel/src/*.asm)
file(GLOB_RECURSE KERNEL_LOADER_ASM_SRC ${PROJECT_SOURCE_DIR}/loader/*.asm)
file(GLOB_RECURSE KERNEL_HEAD ${PROJECT_SOURCE_DIR}/kernel/include/*.h)

file(GLOB_RECURSE KERNEL_LIB_SRC ${PROJECT_SOURCE_DIR}/kernel/src/lib/*.c)
file(GLOB_RECURSE KERNEL_LIB_HEAD ${PROJECT_SOURCE_DIR}/kernel/include/lib/*.h)
file(GLOB_RECURSE KERNEL_LIB_ASM_SRC ${PROJECT_SOURCE_DIR}/kernel/src/lib/*.asm)

# === Add executable... ===
add_executable(kernel ${KERNEL_LOADER_ASM_SRC} ${KERNEL_ASM_SRC} ${KERNEL_SRC} ${KERNEL_HEAD})

# === Add includes ===
target_include_directories(kernel PRIVATE "kernel/include")

# === Add some debug info for NASM ===
set(CMAKE_ASM_NASM_FLAGS_DEBUG "-Fdwarf -32")

# === Setup compile options for KERNEL ===
target_compile_options(
    kernel
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
    $<$<COMPILE_LANGUAGE:C>:-nostdinc>
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    -g -Wall) # compile without linux gcc shit
target_compile_definitions(kernel PRIVATE "KERNEL=1" "_DEBUG=1" "KERNEL_VERSION=\"v0.1.2\"")
target_link_options(kernel PRIVATE -ffreestanding -nostdlib -g -lgcc -T ${CMAKE_SOURCE_DIR}/loader/linker.ld) #link everything using linkerscript

# === Setup output dirs for KERNEL ===
set_target_properties(kernel
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


# === Add progs subdir ===
add_subdirectory(prog)
add_custom_target(initrd_build)
add_dependencies(initrd_build programms)

# === Add dependencies ===
add_dependencies(kernel initrd_build)

# === Commands for initrd build ===
add_custom_command(TARGET initrd_build POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/initrd_base ${CMAKE_BINARY_DIR}/initrd)
add_custom_command(TARGET initrd_build POST_BUILD COMMAND python ${CMAKE_SOURCE_DIR}/tools/initrd_builder.py -o ${CMAKE_BINARY_DIR}/initrd_file -i ${CMAKE_BINARY_DIR}/initrd)

# generate iso file for bochs
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/iso/boot/$<TARGET_FILE_NAME:kernel>)
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/initrd_file ${CMAKE_BINARY_DIR}/iso/boot/initrd)
add_custom_command(TARGET kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/grub ${CMAKE_BINARY_DIR}/iso/boot/grub)
# add_custom_command(TARGET kernel POST_BUILD COMMAND genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A ${PROJECT_NAME} -input-charset utf8 -quiet -boot-info-table -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso ${CMAKE_BINARY_DIR}/iso)
# if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
#     add_custom_command(TARGET kernel POST_BUILD COMMAND grub-mkrescue -o  ${CMAKE_BINARY_DIR}/grub.iso  ${CMAKE_BINARY_DIR}/iso)
#     add_custom_command(TARGET kernel POST_BUILD COMMAND xz -zfkv -T 0 ${CMAKE_BINARY_DIR}/grub.iso)
# ENDIF(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
# 


# === Generate debug info for bocks ===
add_custom_command(TARGET kernel POST_BUILD COMMAND objcopy --only-keep-debug $<TARGET_FILE:kernel> ${CMAKE_BINARY_DIR}/kernel.sym.bin)
add_custom_command(TARGET kernel POST_BUILD COMMAND bash -c "nm ${CMAKE_BINARY_DIR}/kernel.sym.bin | grep \" [Tbt] \" | awk '{ print \$1\" \"\$3 }' > ${CMAKE_BINARY_DIR}/kernel.sym " VERBATIM)

# === Commands for initrd build ===
add_custom_target(reidd COMMAND COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/initrd_base ${CMAKE_BINARY_DIR}/initrd)
add_custom_target(reini COMMAND python ${CMAKE_SOURCE_DIR}/tools/initrd_builder.py -o ${CMAKE_BINARY_DIR}/initrd_file -i ${CMAKE_BINARY_DIR}/initrd)

# === Setup based qemu params ===
set(QEMU_PARAMS -m 64 -kernel $<TARGET_FILE:kernel> -initrd ${CMAKE_BINARY_DIR}/initrd_file) # -usb -device usb-ehci,id=ehci -device usb-kbd -device pci-hellodev

# === Commands for running qemu: ===
# run   - just qemu
# rung  - qemu with debug socket listen
# rungs - qemu with debug socket listen & wait for debugger
# runc  - qemu in cli mode (interaction with kernel over serial interface)
# runcr - qemu in cli mode (interaction with kernel over serial interface) without reboot
add_custom_target(run   COMMAND qemu-system-i386                                ${QEMU_PARAMS} DEPENDS initrd_build kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(rung  COMMAND qemu-system-i386 -s                             ${QEMU_PARAMS} DEPENDS initrd_build kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(rungs COMMAND qemu-system-i386 -s -S                          ${QEMU_PARAMS} DEPENDS initrd_build kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(runc  COMMAND qemu-system-i386          -nographic            ${QEMU_PARAMS} DEPENDS initrd_build kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_target(runcr COMMAND qemu-system-i386          -nographic -no-reboot ${QEMU_PARAMS} DEPENDS initrd_build kernel WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
