//4-1-naskfunc.asmとリンク
//メモリに直書き

void io_hlt(void);
void write_mem8(int addr,int data);

void main(void){
    for(int i=0xa0000;i<=0xaffff;i++){
        write_mem8(i,10);
    }
    while(1<2){
        io_hlt();
     }
}
