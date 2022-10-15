# General config
CORES = 6
BUILD_DIR = ./build
LIBR_BUILD_DIR = ./build_libr

TOOLCHAIN_PATH = $(HOME)/opt/cross
AUTOTOOLS_PATH = $(HOME)/opt/autotools
TOOLCHAIN_BUILD_PATH = $(HOME)/build/cross

TARGET=i686-rkernel

PATCH_ARGS += # --dry-run

# Package versions.
BINUTILS_VERSION = 2.38
GCC_VERSION = 9.5.0

# this needed only if you crazy and want to modify .am/.ac/etc files
AUTOCONF_VERSION = 2.69
AUTOMAKE_VERSION = 1.15.1

# some path calculations...
TOOLCHAIN_BUILD_PATH_SRC = $(TOOLCHAIN_BUILD_PATH)/src

# paths, where the srcs are unpacked
BINUTILS_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/binutils-$(BINUTILS_VERSION)
GCC_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/gcc-$(GCC_VERSION)
# and for autoconf/automake
AUTOCONF_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/autoconf-$(AUTOCONF_VERSION)
AUTOMAKE_SRC_DIR = $(TOOLCHAIN_BUILD_PATH)/automake-$(AUTOMAKE_VERSION)

# paths, where all of that cross-shit are builded
BINUTILS_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-binutils
GCC_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-gcc
AUTOCONF_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-autoconf
AUTOMAKE_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-automake

LIBPHOBOS_BUILD_DIR = $(TOOLCHAIN_BUILD_PATH)/build-libphobos

# config for GCC
GCC_CONFIGURE_FLAGS = --disable-nls --disable-tls --enable-languages=c,c++,d \
					  --disable-libssp --disable-libgomp --disable-libmudflap \
					  --disable-bootstrap --disable-multilib --disable-default-pie \
					  --disable-libphobos --with-system-zlib \
					  --disable-threads \
					  --disable-shared --enable-static

ROOTDIR = $(realpath .)

# just glob files for make works better
LIBR_FILES = $(shell find libr -type f \( -name '*.h' -o -name '*.c' -o -name '*.asm' \) )

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
				  
# yep, in MACOS we don't have gmp/mpc/mprf in normal condition, so that hack exists...
ifeq ($(UNAME_S),Darwin)
GCC_CONFIGURE_FLAGS += --with-gmp=/opt/homebrew --with-mpc=/opt/homebrew --with-mprf=/opt/homebrew
endif

.PHONY: download_toolchain download_autotools build_autotools patch build_cross clean_cross distclean_cross cmake_libr libr install-libr cmake build clean

.DEFAULT: none
none: 
	@echo "Nothing selected."
	@echo "Valid actions are:"
	@echo "	download_toolchain download_autotools build_autotools patch build_cross clean_cross distclean_cross cmake_libr libr install-libr cmake build clean"
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

$(TOOLCHAIN_BUILD_PATH_SRC)/autoconf-$(AUTOCONF_VERSION).tar.xz:
	mkdir -p $(TOOLCHAIN_BUILD_PATH_SRC)
	wget https://ftp.gnu.org/gnu/autoconf/autoconf-$(AUTOCONF_VERSION).tar.xz -O $(TOOLCHAIN_BUILD_PATH_SRC)/autoconf-$(AUTOCONF_VERSION).tar.xz

$(TOOLCHAIN_BUILD_PATH_SRC)/automake-$(AUTOMAKE_VERSION).tar.xz:
	mkdir -p $(TOOLCHAIN_BUILD_PATH_SRC)
	wget https://ftp.gnu.org/gnu/automake/automake-$(AUTOMAKE_VERSION).tar.xz -O $(TOOLCHAIN_BUILD_PATH_SRC)/automake-$(AUTOMAKE_VERSION).tar.xz

$(BINUTILS_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/binutils-$(BINUTILS_VERSION).tar.xz

$(GCC_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/gcc-$(GCC_VERSION).tar.xz

	# we need to initialize git repo there to track changes
	# and for better patch work
	cd $(GCC_SRC_DIR) && \
	git init . && \
	git add . && \
	git commit -m "init"

$(AUTOCONF_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/autoconf-$(AUTOCONF_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/autoconf-$(AUTOCONF_VERSION).tar.xz

$(AUTOMAKE_SRC_DIR): $(TOOLCHAIN_BUILD_PATH_SRC)/automake-$(AUTOMAKE_VERSION).tar.xz
	tar -C $(TOOLCHAIN_BUILD_PATH) -xJf $(TOOLCHAIN_BUILD_PATH_SRC)/automake-$(AUTOMAKE_VERSION).tar.xz

.PHONY: download_toolchain
download_toolchain: $(BINUTILS_SRC_DIR) $(GCC_SRC_DIR)

.PHONY: download_autotools
download_autotools: $(AUTOCONF_SRC_DIR) $(AUTOMAKE_SRC_DIR)

.PHONY: build_autotools
build_autotools: download_autotools
	mkdir -p $(AUTOTOOLS_PATH)

	@echo "[+] Building autoconf"
	export PATH=$(AUTOTOOLS_PATH)/bin:$$PATH && \
	mkdir -p $(AUTOCONF_BUILD_DIR) && \
	cd $(AUTOCONF_BUILD_DIR) && \
	$(AUTOCONF_SRC_DIR)/configure --prefix="$(AUTOTOOLS_PATH)" && \
	$(MAKE) -j $(CORES) && \
	$(MAKE) install

	@echo "[+] Building automake"
	export PATH=$(AUTOTOOLS_PATH)/bin:$$PATH && \
	mkdir -p $(AUTOMAKE_BUILD_DIR) && \
	cd $(AUTOMAKE_BUILD_DIR) && \
	$(AUTOMAKE_SRC_DIR)/configure --prefix="$(AUTOTOOLS_PATH)" && \
	$(MAKE) -j $(CORES) && \
	$(MAKE) install

.PHONY: patch
patch: download_toolchain utils/patches/binutils-$(BINUTILS_VERSION).patch utils/patches/gcc-$(GCC_VERSION)-1.patch utils/patches/gcc-$(GCC_VERSION)-2.patch utils/patches/gcc-9.5.0.macos.patch
	@echo [+] Patching binutils
	# patch -p1 $(PATCH_ARGS) -d $(BINUTILS_SRC_DIR) < utils/patches/binutils-$(BINUTILS_VERSION).patch

    ifeq (${GCC_VERSION},9.5.0)
    ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_P),arm)
	@echo [!] Found darwin/arm, so we need to patch GCC!
	@echo [+] Patching GCC with MACOS M1 patch
	patch -p1 $(PATCH_ARGS) -d $(GCC_SRC_DIR) < utils/patches/gcc-9.5.0.macos.patch
	git -C $(GCC_SRC_DIR) add .
	git -C $(GCC_SRC_DIR) commit -m "macos thing"
    endif
    endif
    endif

	@echo [+] Patching gcc
	patch -p1 $(PATCH_ARGS) -d $(GCC_SRC_DIR) < utils/patches/gcc-$(GCC_VERSION)-1.patch
	patch -p1 $(PATCH_ARGS) -d $(GCC_SRC_DIR) < utils/patches/gcc-$(GCC_VERSION)-2.patch

	@echo [+] Doing autotools thing
	export PATH=$(AUTOTOOLS_PATH)/bin:$$PATH && \
	cd $(GCC_SRC_DIR)/libphobos && \
	autoconf && automake

	export PATH=$(AUTOTOOLS_PATH)/bin:$$PATH && \
	cd $(GCC_SRC_DIR)/libstdc++-v3 && \
	autoconf && automake

.PHONY: prepare_patch
prepare_patch:
	@echo "[+] Preparing for patch exporting..."

	git -C $(GCC_SRC_DIR) add .

	git -C $(GCC_SRC_DIR) diff HEAD -p \
	config.sub fixincludes \
	gcc/config.gcc gcc/config/* libgcc/config.host \
	libstdc++-v3/crossconfig.m4 \
	> utils/patches/gcc-$(GCC_VERSION)-1.patch	

	git -C $(GCC_SRC_DIR) diff HEAD -p \
	libphobos/libdruntime/Makefile.am \
	libphobos/libdruntime/core libphobos/libdruntime/gcstub \
	libphobos/src/Makefile.am \
	> utils/patches/gcc-$(GCC_VERSION)-2.patch

	git -C $(GCC_SRC_DIR) restore --staged .

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

.PHONY: build_libphobos
build_libphobos: download_toolchain libr
	@echo "[+] Building libphobos"
	export PATH=$(TOOLCHAIN_PATH)/bin:$$PATH && \
	export GDC=$(TARGET)-gdc && \
	export LD=$(TARGET)-ld && \
	export AR=$(TARGET)-ar && \
	export RANLIB=$(TARGET)-ranlib && \
	export GDCFLAGS="-fno-moduleinfo -fno-assert -fbounds-check=off" && \
	mkdir -p $(LIBPHOBOS_BUILD_DIR) && \
	cd $(LIBPHOBOS_BUILD_DIR) && \
	$(GCC_SRC_DIR)/libphobos/configure --target=$(TARGET) --prefix="$(TOOLCHAIN_PATH)" \
	--disable-werror --without-pic \
	--disable-shared --enable-static \
	--with-gnu-ld \
	--enable-libphobos --disable-druntime-gc --disable-unix \
	--without-libatomic --without-libbacktrace \
	--with-target-system-zlib=yes \
	--with-cross-host=arm-apple-darwin21.5.0 && \
	$(MAKE) -j $(CORES) && \
	$(MAKE) install && \
	echo "[!] libphobos builded!!"

.PHONY: clean_cross
clean_cross:
	mkdir -p $(TOOLCHAIN_PATH)
	@echo "[!] Cleaning cross-compiler files"
	rm -rf $(TOOLCHAIN_PATH)/*

.PHONY: distclean_cross
distclean_cross:
	mkdir -p $(TOOLCHAIN_PATH)
	@echo "[!] Cleaning cross-compiler files"

	cd $(BINUTILS_BUILD_DIR) && \
	$(MAKE) clean distclean; \
	rm ./config.cache

	cd $(GCC_BUILD_DIR) && \
	$(MAKE) clean distclean; \
	rm ./config.cache

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
	ln -s $(ROOTDIR)/$(LIBR_BUILD_DIR)/lib/libr.a $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)
	ln -s $(ROOTDIR)/$(LIBR_BUILD_DIR)/CMakeFiles/r.dir/src/crt0.asm.obj $(TOOLCHAIN_PATH)/lib/gcc/i686-rkernel/$(GCC_VERSION)/crt0.o
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

.PHONY: runc
runc: cmake
	$(MAKE) -C $(BUILD_DIR) -j $(CORES) runc 

.PHONY: runc
rung: cmake
	# tmux split-window -h "sleep 2; r2 -i r2_init -e dbg.exe.path='./build/bin/kernel' -a x86.nasm -b 32 -D gdb gdb://localhost:1234"
	tmux split-window -h "sleep 2; lldb --local-lldbinit --arch i386"
	$(MAKE) -C $(BUILD_DIR) -j $(CORES) runc

.PHONY: clean
clean: 
	@echo "Clean"
	rm -rf $(BUILD_DIR)/*
