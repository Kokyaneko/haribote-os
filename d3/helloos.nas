; hello-os
; day 2
; TAB=4

    ORG 0x07c00  ;固定のOS読み込み用メモリ
    JMP entry
    DB 0x90
    DB "HELLOIPL"      ; ブートセクタの名前（8バイト）
    DW 512             ; 1セクタの大きさ
    DB 1               ; クラスタの大きさ
    DW 1               ; FATの開始セクタ
    DB 2               ; FATの個数
    DW 224             ; ルートディレクトリの領域の大きさ
    DW 2880            ; このドライブの大きさ
    DB 0xf0            ; メディアのタイプ
    DW 9               ; FAT領域の長さ
    DW 18              ; 1トラックのセクタ数
    DW 2               ; ヘッドの数
    DD 0               ; パーティション未使用
    DD 2880            ; ドライブの大きさ（再掲）
    DB 0, 0, 0x29
    DD 0xffffffff
    DB "HELLO-OS   "   ; ディスクの名前（11バイト）
    DB "FAT12   "      ; フォーマットの名前（8バイト）
    RESB    18              ; ここまでで合計62バイトのヘッダ
entry:
        MOV AX,0
        MOV SS,AX
        MOV SP,0x07c00
        MOV DS,AX
        MOV ES,AX
        MOV SI,msg
putloop:
        MOV AL,[SI]
        ADD SI,1
        CMP AL,0
        JE fin          ;何も起きていないならHLT指示
        MOV AH,0x0e     ;文字表示
        MOV BX,15
        INT 0x10        ;INTでBIOSを呼んで表示させる
        JMP putloop
fin:
        HLT
        JMP fin         ;無限ループ
msg:
    DB 0x0a , 0x0a      ;0x0a : 改行
    DB "Hello World"
    DB 0x0a             ;0x0a : 改行
    DB 0

    RESB    510-($-$$) ; 512バイトの最後までを0x00で埋め、最後にブートセクタの目印を置く
    DB      0x55, 0xaa
