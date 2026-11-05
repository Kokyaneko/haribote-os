    MOV AX,0x0820
    MOV ES,AX
    MOV CH,0
    MOV DH,0
    MOV CL,2

    MOV AH,0X02     ;read disk
    MOV AL,1
    MOV BX,0
    MOV DL,0X00
    INT 0x13
    JC error
