mkdir build;
cd build && 
cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/rubikoid/gcc_cross/toolchain.cmake &&
cd .. &&
make -C build -j 8
