# GCC

## Build cross-compiler

### Get code

```bash
mkdir cross_comp
wget https://ftp.gnu.org/gnu/binutils/binutils-2.36.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-11.1.0/gcc-11.1.0.tar.xz
tar -xJf binutils-*.tar.xz
tar -xJf gcc-*.tar.xz
```

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

```bash
mkdir build-binutils
cd build-binutils
../binutils-*/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j 16
make install
```

```bash
mkdir build-gcc
cd build-gcc
../gcc-*/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++,d --disable-libssp --disable-libgomp --disable-libmudflap --disable-bootstrap --disable-multilib --disable-default-pie --with-newlib --enable-libphobos --without-libbacktrace --without-libatomic --with-target-system-zlib=yes --with-system-zlib
make all-gcc -j 16
make all-target-libgcc -j 16
make all-target-libphobos -j 16
make install-gcc
make install-target-libgcc
```
