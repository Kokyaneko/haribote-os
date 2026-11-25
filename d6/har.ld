OUTPUT_FORMAT("binary");

SECTIONS
{
    /* bootpack.bin のロード先（セグメント2のベースアドレス 0x00280000 からの相対オフセット 0） */
    . = 0x00000000;

    .text : {
        /* 最初に実行される main() を含む bootpack.o を必ず先頭に配置する */
        *bootpack.o(.text .text.*)
        *(.text .text.*)
    }

    /* 文字列リテラル（"ABC 123" やフォントデータなど）と初期値あり変数 */
    .data ALIGN(4) : {
        *(.rodata .rodata.*)
        *(.data .data.*)
    }

    /* 初期値なし変数（static変数など） */
    .bss ALIGN(4) : {
        *(.bss .bss.*)
        *(COMMON)
    }

    /* 不要なセクションを除外してバイナリ肥大化・ズレを防ぐ */
    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note*)
    }
}
