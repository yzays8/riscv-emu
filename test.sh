#!/bin/bash

readonly filename="test/add-addi"

make
clang -Wl,-Ttext=0x0 -nostdlib --target=riscv64 -march=rv64g -mno-relax -o $filename $filename.s
llvm-objcopy -O binary $filename $filename.bin
make run FILE=$filename.bin
