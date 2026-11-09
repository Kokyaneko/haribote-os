//4-3-naskfunc.asmとリンク
//メモリに直書き
//chose color

void io_hlt(void);

void io_cli(void);
void io_cut8(int port,int data);
void io_load_eflags(void);
void io_store_eflags(int flags);

void init_pallette(void);
void set_pallette(int start,int end,unsigned char *rgb);

void main(void){
    while(1<2){
        io_hlt();
     }
}
