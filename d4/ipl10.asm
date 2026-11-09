    ORG     0x7c00          ; 読み込みアドレスを指定

    ; 初期設定
    MOV     AX,0x0820       ; メモリセグメント設定
    MOV     ES,AX
    MOV     CH,0            ; シリンダ 0
    MOV     DH,0            ; ヘッド 0
    MOV     CL,2            ; セクタ 2

readloop:
    MOV     SI,0            ; リトライ回数初期化

retry:
    MOV     AH,0x02         ; ディスク読み出しBIOS
    MOV     AL,1            ; 1セクタ
    MOV     BX,0            ; 読み込み先オフセット
    MOV     DL,0x00         ; Aドライブ
    INT     0x13
    JNC     next            ; 成功なら次へ
    
    ; エラーリトライ処理
    ADD     SI,1
    CMP     SI,5            ; 5回でエラー
    JAE     error
    MOV     AH,0x00
    MOV     DL,0x00
    INT     0x13
    JMP     retry

next:
    CYLS EQU 10             ; 読み込みシリンダ数
    MOV BYTE [0x0ff0],CYLS  ; 他のプログラムに共有
    MOV     AX,ES
    ADD     AX,0x0020       ; セグメントを512バイト分進める
    MOV     ES,AX
    
    ADD     CL,1
    CMP     CL,18           ; 18セクタまで繰り返す
    JBE     readloop

    MOV     CL,1
    ADD     DH,1
    CMP     DH,2            ; 裏面（ヘッド1）まで繰り返す
    JB      readloop

    MOV     DH,0
    ADD     CH,1            ; 次のシリンダへ
    CMP     CH,CYLS
    JB      readloop

    ; OS本体へジャンプ
    JMP     0x0820:0x0000

error:
    msg:
    DB 0x0a , 0x0a      ;0x0a : 改行
    DB "Error!"
    DB 0x0a             ;0x0a : 改行
    DB 0
fin:
    HLT
    JMP     fin

    RESB    510-($-$$)
    DB      0x55, 0xaa      ; ブートシグネチャ
