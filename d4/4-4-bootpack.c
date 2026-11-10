//4-4-naskfunc.asmとリンク
//メモリに直書き
//chose and set color

void io_hlt(void);

void io_cli(void);
void io_out8(int port,int data);
int io_load_eflags(void);
void io_store_eflags(int flags);

void init_pallette(void);
void set_pallette(int start,int end,unsigned char *rgb);

void main(void){
    init_pallette();
    char *p = (char *) 0xa0000;
    for(int i=0;i<=0xffff;i++) p[i] = i & 0x0f;
    while(1<2){
        io_hlt();
     }
}

void init_pallette(void){
    static unsigned char table_rgb[16*3] = {
        0x00, 0x00, 0x00, // black
        0xff, 0x00, 0x00, // light red
        0x00, 0xff, 0x00, // light green
        0xff, 0xff, 0x00, // light yellow
        0x00, 0x00, 0xff, // light blue
        0xff, 0x00, 0xff, // light purple
        0x00, 0xff, 0xff, // light water
        0xff, 0xff, 0xff, // white
        0xc6, 0xc6, 0xc6, // light gray
        0x84, 0x00, 0x00, // dark red
        0x00, 0x84, 0x00, // dark green
        0x84, 0x84, 0x00, // dark yellow
        0x00, 0x00, 0x84, // dark blue
        0x84, 0x00, 0x84, // dark purple
        0x00, 0x84, 0x84, // dark water
        0x84, 0x84, 0x84  // dark gray
    };
    set_pallette(0,15,table_rgb);
    return;
}
void set_pallette(int start,int end,unsigned char *rgb){
    int eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8,start);
    for(int i=start;i<=end;i++){
        io_out8(0x03c9 ,rgb[0] / 4);
        io_out8(0x03c9 ,rgb[1] / 4);
        io_out8(0x03c9 ,rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags);
    return;
}
