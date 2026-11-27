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

needs_rebuild() {
    local out="$1"; shift
    [ ! -f "$out" ] && return 0
    local f
    for f in "$@"; do
        if [ -f "$f" ] && [ "$f" -nt "$out" ]; then
            return 0
        fi
    done
    return 1
}

# 1. ipl10.bin のビルド
if needs_rebuild ipl10.bin ipl10.asm; then
    echo "[+] Assembling ipl10.asm"
    nasm -f bin ipl10.asm -o ipl10.bin
fi

# 2. asmhead.bin のビルド
if needs_rebuild asmhead.bin asmhead.asm; then
    echo "[+] Assembling asmhead.asm"
    nasm -f bin asmhead.asm -o asmhead.bin
fi

# 3. アセンブラ関数 (naskfunc.o) のビルド
if needs_rebuild naskfunc.o naskfunc.asm; then
    echo "[+] Compiling naskfunc.asm"
    nasm -f elf32 naskfunc.asm -o naskfunc.o
fi

# 4. フォントファイル (hankaku.c / hankaku.o) のビルド
#    ※ 旧版は hankaku.c と hankaku.o の比較が無く、
#      hankaku.c を直接編集しても再コンパイルされなかった。
if [ -f hankaku.txt ]; then
    if needs_rebuild hankaku.c hankaku.txt; then
        echo "[+] Generating font data"
        python3 makefont.py hankaku.txt hankaku.c
    fi
    if needs_rebuild hankaku.o hankaku.c; then
        echo "[+] Compiling hankaku.c"
        gcc -m32 -fno-pie -nostdlib -c hankaku.c -o hankaku.o
    fi
fi

# 5. C言語のビルド
#   bootpack.c のコンパイル
if needs_rebuild "${BOOTPACK_O}" "${BOOTPACK_C}" set.h; then
    echo "[+] Compiling ${BOOTPACK_C}"
    gcc -m32 -fno-pie -fno-asynchronous-unwind-tables -nostdlib -c "${BOOTPACK_C}" -o "${BOOTPACK_O}"
fi
#   graphic.c のコンパイル
if needs_rebuild graphic.o graphic.c set.h; then
    echo "[+] Compiling graphic.c"
    gcc -m32 -fno-pie -fno-asynchronous-unwind-tables -nostdlib -c graphic.c -o graphic.o
fi

# 6. リンカによる結合 (har.ld を使用)
LINK_DEPS=( "${BOOTPACK_O}" graphic.o naskfunc.o har.ld )
[ -f hankaku.o ] && LINK_DEPS+=( hankaku.o )

if needs_rebuild "${BOOTPACK_BIN}" "${LINK_DEPS[@]}"; then
    echo "[+] Linking ${BOOTPACK_BIN}"
    if [ -f hankaku.o ]; then
        ld -m elf_i386 -T har.ld -e main -o "${BOOTPACK_BIN}" \
            "${BOOTPACK_O}" graphic.o naskfunc.o hankaku.o
    else
        ld -m elf_i386 -T har.ld -e main -o "${BOOTPACK_BIN}" \
            "${BOOTPACK_O}" graphic.o naskfunc.o
    fi
fi

# 7. sysファイルの作成
if needs_rebuild "${SYS_FILE}" asmhead.bin "${BOOTPACK_BIN}"; then
    echo "[+] Creating ${SYS_FILE}"
    cat asmhead.bin "${BOOTPACK_BIN}" > "${SYS_FILE}"
fi

# 8. フロッピーイメージの作成
if needs_rebuild "${IMG_FILE}" ipl10.bin "${SYS_FILE}"; then
    echo "[+] Generating floppy image (${IMG_FILE})"
    dd if=/dev/zero of="${IMG_FILE}" bs=512 count=2880 status=none
    dd if=ipl10.bin   of="${IMG_FILE}" bs=512 seek=0 conv=notrunc status=none
    dd if="${SYS_FILE}" of="${IMG_FILE}" bs=512 seek=1 conv=notrunc status=none
fi

# 9. QEMU起動
echo "[+] Starting QEMU"
qemu-system-i386 -fda "${IMG_FILE}"
