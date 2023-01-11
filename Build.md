# Build cross-compiler

Reqired software (generally, `base-devel`):

- Working compiler for your host system (gcc/llvm/anything you want, i don't care)
- `make` - to run make
- `wget` - to download gcc/binutils/automake source code
- `tar`, with `gzip` - to extract srcs
- `git` - for patches generation
- `patch` - for applying patches over gcc/binutils src
- `cmake` - for building kernel
- `qemu` - for testing kernel

Firstly, you need to download and patch cross-compiler (and download and build autotools. Because of GCC.):

`make patch`

Then you need to build crosscompiler itself:

`make build_cross`

Then you need to build libr (libc replacement), and install it to cross-compiler env:

`make install_libr`

Then you need to build libphobos (Dlang runtime):

`make build_libphobos`

# Bulid kernel

To build kernel, run:

`make cmake`

This will generate cmake build directory in `./build`

# Run kernel

`make runc`
