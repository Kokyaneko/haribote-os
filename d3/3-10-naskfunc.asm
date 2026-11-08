;nascfanc

[BITS 32]

; for object file
GLOBAL io_hlt      ;グローバル関数

; 関数の動作
[SECTION .text]         ;オブジェクトファイルではこれを書いてからプログラムを書く
io_hlt:
    HLT
    RET                 ;返り値
