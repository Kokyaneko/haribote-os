#!/bin/bash
set -e

# イメージ作成対象を,A-Bの形で定義できる.
A=6
B=6

# ファイル名を自動で組み立て
BOOTPACK_C="${A}-${B}-bootpack.c"
BOOTPACK_O="${A}-${B}-bootpack.o"
BOOTPACK_BIN="${A}-${B}-bootpack.bin"
SYS_FILE="${A}-${B}-haribote.sys"
IMG_FILE="${A}-${B}-haribote.img"

echo "=== Building Haribote OS (${A}-${B}) ==="

# 1. ipl10.bin のビルド
if [ ! -f ipl10.bin ] || [ ipl10.asm -nt ipl10.bin ]; then
    echo "[+] Assembling ipl10.asm"
    nasm -f bin ipl10.asm -o ipl10.bin
fi

# 2. asmhead.bin のビルド
if [ ! -f asmhead.bin ] || [ asmhead.asm -nt asmhead.bin ]; then
    echo "[+] Assembling asmhead.asm"
    nasm -f bin asmhead.asm -o asmhead.bin
fi

# 3. アセンブラ関数 (naskfunc.o) のビルド
if [ ! -f naskfunc.o ] || [ naskfunc.asm -nt naskfunc.o ]; then
    echo "[+] Compiling naskfunc.asm"
    nasm -f elf32 naskfunc.asm -o naskfunc.o
fi

# 4. フォントファイル (hankaku.c / hankaku.o) のビルド
if [ -f "hankaku.txt" ] && ( [ ! -f "hankaku.o" ] || [ hankaku.txt -nt hankaku.c ] ); then
    echo "[+] Generating font data"
    python3 makefont.py hankaku.txt hankaku.c
    gcc -m32 -fno-pie -nostdlib -c hankaku.c -o hankaku.o
fi

# 5. C言語のビルド
#   bootpack.c のコンパイル
if [ ! -f "${BOOTPACK_O}" ] || [ "${BOOTPACK_C}" -nt "${BOOTPACK_O}" ] || [ -f "set.h" ] && [ "set.h" -nt "${BOOTPACK_O}" ]; then
    echo "[+] Compiling ${BOOTPACK_C}"
    gcc -m32 -fno-pie -fno-asynchronous-unwind-tables -nostdlib -c "${BOOTPACK_C}" -o "${BOOTPACK_O}"
fi
#    graphic.c のコンパイル
if [ ! -f "graphic.o" ] || [ "graphic.c" -nt "graphic.o" ] || [ -f "set.h" ] && [ "set.h" -nt "graphic.o" ]; then
    echo "[+] Compiling graphic.c"
    gcc -m32 -fno-pie -fno-asynchronous-unwind-tables -nostdlib -c graphic.c -o graphic.o
fi

# 6. リンカによる結合 (har.ld を使用)
if [ ! -f "${BOOTPACK_BIN}" ] || [ "${BOOTPACK_O}" -nt "${BOOTPACK_BIN}" ] || [ graphic.o -nt "${BOOTPACK_BIN}" ] || [ naskfunc.o -nt "${BOOTPACK_BIN}" ] || [ har.ld -nt "${BOOTPACK_BIN}" ] || ( [ -f "hankaku.o" ] && [ hankaku.o -nt "${BOOTPACK_BIN}" ] ); then
    echo "[+] Linking ${BOOTPACK_BIN}"
    if [ -f "hankaku.o" ]; then
        ld -m elf_i386 -T har.ld -e main -o "${BOOTPACK_BIN}" "${BOOTPACK_O}" graphic.o naskfunc.o hankaku.o
    else
        ld -m elf_i386 -T har.ld -e main -o "${BOOTPACK_BIN}" "${BOOTPACK_O}" graphic.o naskfunc.o
    fi
fi

# 7. sysファイルの作成
if [ ! -f "${SYS_FILE}" ] || [ asmhead.bin -nt "${SYS_FILE}" ] || [ "${BOOTPACK_BIN}" -nt "${SYS_FILE}" ]; then
    echo "[+] Creating ${SYS_FILE}"
    cat asmhead.bin "${BOOTPACK_BIN}" > "${SYS_FILE}"
fi

# 8. フロッピーイメージの作成
echo "[+] Generating floppy image (${IMG_FILE})"
dd if=/dev/zero of="${IMG_FILE}" bs=512 count=2880 status=none
dd if=ipl10.bin of="${IMG_FILE}" bs=512 seek=0 conv=notrunc status=none
dd if="${SYS_FILE}" of="${IMG_FILE}" bs=512 seek=1 conv=notrunc status=none

# 9. QEMU起動
echo "[+] Starting QEMU"
qemu-system-i386 -fda "${IMG_FILE}"
