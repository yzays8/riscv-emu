#!/bin/bash

readonly TEST_DIR=test

function make_bin() {
    for dir in $TEST_DIR/*; do
        for f in $dir/*; do
            local file_ext=${f##*.}
            case $file_ext in
                c)
                    local filename=${f%.*}
                    clang -S $f -nostdlib --target=riscv64 -march=rv64i -mabi=lp64 -mno-relax -o $filename.s
                    clang -Wl,-Ttext=0x0 -nostdlib --target=riscv64 -mabi=lp64 -march=rv64i -mno-relax -o $filename $filename.s
                    llvm-objcopy -O binary $filename $filename.bin
                    ;;
                s)
                    local filename=${f%.*}
                    clang -Wl,-Ttext=0x0 -nostdlib --target=riscv64 -mabi=lp64 -march=rv64i -mno-relax -o $filename $filename.s
                    llvm-objcopy -O binary $filename $filename.bin
                    ;;
            esac
        done
    done
}

make
if [ $? -ne 0 ]; then
    echo "make failed" >&2
    exit 1
fi

make_bin

if [ $? -eq 0 ]; then
    make test
fi
