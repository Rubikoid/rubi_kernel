UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)

# PATCH_ARGS += --dry-run

CORES = 6
BUILD_DIR = ./build
LIBR_BUILD_DIR = ./build_libr

BINUTILS_VERSION = 2.38
GCC_VERSION = 9.5.0

TARGET=i686-rkernel

TOOLCHAIN_PATH = $(HOME)/opt/cross
TOOLCHAIN_BUILD_PATH = $(HOME)/build/cross
TOOLCHAIN_BUILD_PATH_SRC = $(TOOLCHAIN_BUILD_PATH)/src

BINUTILS_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/binutils-$(BINUTILS_VERSION)
GCC_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/gcc-$(GCC_VERSION)

BINUTILS_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-binutils
GCC_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-gcc

GCC_CONFIGURE_FLAGS = --disable-nls --enable-languages=c,c++,d \
					  --disable-libssp --disable-libgomp --disable-libmudflap \
					  --disable-bootstrap --disable-multilib --disable-default-pie \
					  --disable-libphobos --with-system-zlib \
					  --disable-threads \
					  --disable-shared --enable-static
					  
ifeq ($(UNAME_S),Darwin)
GCC_CONFIGURE_FLAGS += --with-gmp=/opt/homebrew --with-mpc=/opt/homebrew --with-mprf=/opt/homebrew
endif

LIBR_FILES = $(shell find libr -type f \( -name '*.h' -o -name '*.c' -o -name '*.asm' \) )

.PHONY: download_toolchain patch build_cross clean_cross cmake_libr libr install-libr cmake build clean

.DEFAULT: none
none: 
	@echo "Nothing selected."
	@echo "Valid actions are:"
	@echo "	download_toolchain patch build_cross clean_cross cmake_libr libr install-libr cmake build clean"
	@echo "=== Some debug: ==="
	@echo "GCC CONFIG FLAGS: " $(GCC_CONFIGURE_FLAGS)
	@echo "==================="
	@echo "LIBR_FILES: " $(LIBR_FILES)

$(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz:
	mkdir -p $(TOOLCHAIN_BUILD_PATH_SRC)
	wget https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.xz -O $(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz

$(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz:
	mkdir -p $(TOOLCHAIN_BUILD_PATH_SRC)
	wget https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz -O $(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz

$(BINUTILS_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz

$(GCC_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz

.PHONY: download_toolchain
download_toolchain: $(TOOLCHAIN_BUILD_PATH)/gcc-$(GCC_VERSION) $(TOOLCHAIN_BUILD_PATH)/binutils-$(BINUTILS_VERSION)

.PHONY: patch
patch: download_toolchain utils/patches/binutils-$(BINUTILS_VERSION).patch utils/patches/gcc-$(GCC_VERSION).patch utils/patches/gcc-9.5.0.macos.patch
	@echo [+] Patching binutils
	patch -p1 $(PATCH_ARGS) -d $(BINUTILS_SRC_DIR) < utils/patches/binutils-$(BINUTILS_VERSION).patch

    ifeq (${GCC_VERSION},9.5.0)
    ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_P),arm)
	@echo [!] Found darwin/arm, so we need to patch GCC!
	@echo [+] Patching GCC with MACOS M1 patch
	patch -p1 $(PATCH_ARGS) -d $(GCC_SRC_DIR) < utils/patches/gcc-9.5.0.macos.patch
    endif
    endif
    endif

	@echo [+] P	atching gcc
	patch -p1 $(PATCH_ARGS) -d $(GCC_SRC_DIR) < utils/patches/gcc-$(GCC_VERSION).patch

.PHONY: build_cross
build_cross: download_toolchain
	@echo "[+] Building cross-compiler"
	mkdir -p $(TOOLCHAIN_PATH)

	@echo "[+] Building binutils"
	export PATH=$(TOOLCHAIN_PATH)/bin:$$PATH && \
	mkdir -p $(BINUTILS_BUILD_DIR) && \
	cd $(BINUTILS_BUILD_DIR) && \
	$(BINUTILS_SRC_DIR)/configure --target=$(TARGET) --prefix="$(TOOLCHAIN_PATH)" --with-sysroot --disable-nls --disable-werror && \
	$(MAKE) -j $(CORES) && \
	$(MAKE) install

	@echo "[+] Building GCC"
	export PATH=$(TOOLCHAIN_PATH)/bin:$$PATH && \
	mkdir -p $(GCC_BUILD_DIR) && \
	cd $(GCC_BUILD_DIR) && \
	$(GCC_SRC_DIR)/configure --target=$(TARGET) --prefix="$(TOOLCHAIN_PATH)" $(GCC_CONFIGURE_FLAGS) && \
	$(MAKE) all-gcc -j $(CORES) && \
	$(MAKE) all-target-libgcc -j $(CORES) && \
	$(MAKE) install-gcc && \
	$(MAKE) install-target-libgcc

.PHONY: clean_cross
clean_cross:
	mkdir -p $(TOOLCHAIN_PATH)
	@echo "[!] Cleaning cross-compiler files"
	rm -rf $(TOOLCHAIN_PATH)/*

.PHONY: cmake_libr
cmake_libr: libr/CMakeLists.txt
	mkdir -p $(LIBR_BUILD_DIR) && \
	cd $(LIBR_BUILD_DIR) && \
	cmake ../libr

.PHONY: libr
libr: cmake_libr $(LIBR_FILES)
	$(MAKE) -C $(LIBR_BUILD_DIR) -j $(CORES)

.PHONY: install-libr
install-libr: libr
	cp $(LIBR_BUILD_DIR)/lib/libr.a $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)
	cp $(LIBR_BUILD_DIR)/CMakeFiles/r.dir/src/crt0.asm.obj $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)/crt0.o
	cp -r libr/include/* $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)/include

.PHONY: uninstall-libr
uninstall-libr: libr
	rm $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)/{libr.a,crt0.o}
	rm -rf $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)/include/{types.h,lib}

.PHONY: cmake
cmake: CMakeLists.txt
	mkdir -p $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	cmake ..

.PHONY: build
build: cmake
	$(MAKE) -C $(BUILD_DIR) -j $(CORES)

.PHONY: clean
clean: 
	@echo "Clean"
	rm -rf $(BUILD_DIR)/*
