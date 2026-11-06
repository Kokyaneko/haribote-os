#!/bin/bash

# 1. バイナリにアセンブル
nasm ipl.asm -o ipl.bin
nasm haribote-main.asm -o haribote-main.bin

# 2. 1.44MBの空のフロッピーディスクイメージを作成
dd if=/dev/zero of=haribote-simple.img bs=512 count=2880

# 3. イメージの先頭（最初の512バイト）に ipl.bin を書き込む
dd if=ipl.bin of=haribote-simple.img bs=512 seek=0 conv=notrunc

# 4. haribote-main.binの書き込み
dd if=haribote-main.bin of=haribote-simple.img bs=512 seek=1 conv=notrunc

# 4. QEMUで起動
qemu-system-i386 -fda haribote-simple.img
