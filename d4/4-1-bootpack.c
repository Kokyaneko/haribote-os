//4-1-naskfunc.asmとリンク
//メモリに直書き

void io_hlt(void);
void write_mem8(int addr,int data);

void main(void){
    for(int i=0xa0000;i<0xafa00;i++){
        write_mem8(i,14);
    }
    while(1<2){
        io_hlt();
     }
}
