; ipl.asm
; TAB=4

    ORG     0x7c00

    MOV     AX, 0x0820
    MOV     ES, AX
    MOV     CH, 0
    MOV     DH, 0
    MOV     CL, 2

    MOV     AH, 0x02        ; ディスク読み出し
    MOV     AL, 1           ; 1セクタ読む
    MOV     BX, 0
    MOV     DL, 0x00        ; Aドライブ
    INT     0x13
    JNC     next            ;問題がないなら次へ進む
    JC      error

next:
    MOV AX,ES
    ADD AX,0x0020
    MOV ES,AX

fin:
    HLT
    JMP     fin

error:
    MOV     SI, msg
error_loop:
    MOV     AL, [SI]
    ADD     SI, 1
    CMP     AL, 0
    JE      error_fin
    MOV     AH, 0x0e
    MOV     BX, 15
    INT     0x10
    JMP     error_loop
error_fin:
    HLT
    JMP     error_fin

msg:
    DB      0x0a, 0x0a
    DB      "load error"
    DB      0x0a
    DB      0

    RESB    510-($-$$)      ; 512バイトまでを0で埋める
    DB      0x55, 0xaa      ; ブートシグネチャ
