#!/bin/bash
set -e

# 1. ブートセクタ (ipl.asm) のアセンブル
nasm -f bin ipl10.asm -o ipl10.bin

# 2. アセンブラ関数 (naskfunc.nas) を32ビットELFオブジェクトに変換
nasm -f elf32 4-1-naskfunc.asm -o 4-1-naskfunc.o

# 3. C言語 (bootpack.c) を32ビット用かつ標準ライブラリなしでコンパイル
gcc -m32 -fno-pie -nostdlib -c 4-1-bootpack.c -o 4-1-bootpack.o

# 4. リンカ (ld) でオブジェクト同士を結合し、純粋なバイナリ (bootpack.hrb) を生成
# ※ ロード番地を指定するため -Ttext 0x00280000 を付与
ld -m elf_i386 -Ttext 0x00280000 --oformat binary -o 4-1-bootpack.hrb 4-1-bootpack.o 4-1-naskfunc.o

# 5. 1.44MBのフロッピーディスクイメージを作成
dd if=/dev/zero of=4-1-haribote.img bs=512 count=2880 status=none

# 6. イメージの先頭（セクタ0）に ipl.bin を書き込む
dd if=ipl10.bin of=4-1-haribote.img bs=512 seek=0 conv=notrunc status=none

# 7. セクタ1以降に bootpack.hrb を書き込む
dd if=4-1-bootpack.hrb of=4-1-haribote.img bs=512 seek=1 conv=notrunc status=none

# 8. QEMUで起動して動作確認
qemu-system-i386 -fda 4-1-haribote.img
