#!/bin/bash

readonly filename="test/fib"

make
if [ $? -eq 0 ]; then
    clang -S $filename.c -nostdlib --target=riscv64 -march=rv64i -mabi=lp64 -mno-relax -o $filename.s
    clang -Wl,-Ttext=0x0 -nostdlib --target=riscv64 -mabi=lp64 -march=rv64i -mno-relax -o $filename $filename.s
    llvm-objcopy -O binary $filename $filename.bin
    make run FILE=$filename.bin
fi
