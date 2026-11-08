;nascfanc

[BITS 32]

; for object file
GLOBAL io_hlt          ;グローバル関数
GLOBAL write_mem8      ;グローバル関数

; 関数の動作
[SECTION .text]         ;オブジェクトファイルではこれを書いてからプログラムを書く
io_hlt:
    HLT
    RET                 ;返り値
write_mem8:
    MOV ECX,[ESP+4]     ; アドレス([ESP+4])読み込み
    MOV AL,[ESP+8]      ; データ([ESP+8])読み込み
    MOV [ECX],AL
    RET
