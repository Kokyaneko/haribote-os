#!/bin/bash

# 1. ipl.asm をバイナリにアセンブル
nasm ipl.asm -o ipl.bin

# 2. 1.44MBの空のフロッピーディスクイメージを作成
dd if=/dev/zero of=helloos.img bs=512 count=2880

# 3. イメージの先頭（最初の512バイト）に ipl.bin を書き込む
dd if=ipl.bin of=helloos.img bs=512 count=1 conv=notrunc

# 4. QEMUで起動
qemu-system-i386 -fda helloos.img
