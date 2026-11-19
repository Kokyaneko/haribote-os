#!/bin/bash
set -e

# 0. フォントファイル (hankaku.txt) がある場合は hankaku.c に変換してコンパイル
if [ -f "hankaku.txt" ] && [ ! -f "hankaku.o" ]; then
    python3 makefont.py hankaku.txt hankaku.c
    gcc -m32 -fno-pie -nostdlib -c hankaku.c -o hankaku.o
fi

# 1. ブートセクタと asmhead をバイナリとしてアセンブル
nasm -f bin ipl10.asm -o ipl10.bin
nasm -f bin asmhead.asm -o asmhead.bin

# 2. C言語とアセンブラ関数をコンパイル＆リンク
nasm -f elf32 5-naskfunc.asm -o 5-naskfunc.o
gcc -m32 -fno-pie -nostdlib -c 5-8-bootpack.c -o 5-8-bootpack.o

# フォントオブジェクト (hankaku.o) が存在する場合は一緒にリンクする
if [ -f "hankaku.o" ]; then
    ld -m elf_i386 -Ttext 0x00280000 -e main --oformat binary -o 5-8-bootpack.bin 5-8-bootpack.o 5-naskfunc.o hankaku.o
else
    ld -m elf_i386 -Ttext 0x00280000 -e main --oformat binary -o 5-8-bootpack.bin 5-8-bootpack.o 5-naskfunc.o
fi

# 3. asmhead.bin と bootpack.bin を結合して 5-8-haribote.sys を作成
cat asmhead.bin 5-8-bootpack.bin > 5-8-haribote.sys

# 4. フロッピーディスクイメージの作成
dd if=/dev/zero of=5-8-haribote.img bs=512 count=2880 status=none

# 5. セクタ0に ipl10.bin を書き込む
dd if=ipl10.bin of=5-8-haribote.img bs=512 seek=0 conv=notrunc status=none

# 6. セクタ1以降に 5-8-haribote.sys を書き込む
dd if=5-8-haribote.sys of=5-8-haribote.img bs=512 seek=1 conv=notrunc status=none

# 7. QEMUで起動
qemu-system-i386 -fda 5-8-haribote.img
