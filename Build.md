# GCC

## Build cross-compiler

### Build automake

```bash
export PREFIX_AUTOMAKE="$HOME/build/cross_comp/autoconf_builded"
export PATH="$PREFIX_AUTOMAKE/bin:$PATH"
```

```bash
wget https://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.xz
wget https://ftp.gnu.org/gnu/automake/automake-1.15.1.tar.xz
tar -xJf src/autoconf-2.69.tar.xz
tar -xJf src/automake-1.15*.tar.xz
```

```bash

```

### Get code

```bash
mkdir cross_comp
wget https://ftp.gnu.org/gnu/binutils/binutils-2.38.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-9.5.0/gcc-9.5.0.tar.xz
tar -xJf src/binutils-2.3*.tar.xz
tar -xJf src/gcc-9*.tar.xz
```

### Setup vars

```bash
export TARGET=i686-rkernel
export PREFIX="$HOME/opt/cross"
export PATH="$PREFIX/bin:$PATH"
```

### Build binutils

```bash
mkdir build-binutils
cd build-binutils
../binutils-*/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j 8
make install
```

### Build gcc

```bash
mkdir build-gcc
cd build-gcc
../gcc-9*/configure "--target=$TARGET" --prefix="$PREFIX" --disable-nls --enable-languages=c,c++,d --disable-libssp --disable-libgomp --disable-libmudflap --disable-bootstrap --disable-multilib --disable-default-pie --disable-libphobos --with-system-zlib --disable-threads --disable-shared --enable-static
make all-gcc -j 8
make all-target-libgcc -j 8
make install-gcc
make install-target-libgcc
```
