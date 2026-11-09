//4-3-naskfunc.asmとリンク
//メモリに直書き
//shima-shima(pointer)

void io_hlt(void);

void main(void){
    for(int i=0xa0000;i<0xafa00;i++){
        char *p;//メモリに1バイト単位でアクセスできる.
        p = (char *)i;
        *p = i & 0x0f;
    }
    while(1<2){
        io_hlt();
     }
}
