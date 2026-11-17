//5-naskfunc.asmとリンク
//BOOT情報を取得
#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

void putfont8(char *vram,int xsize, int x, int y, char c, char *font);

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    char *vram;
};

void main(void){
    static char font_A[16] = {
        0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
        0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
    };
   struct BOOTINFO *binfo;
    
    init_palette();
    binfo = (struct BOOTINFO *) 0x0ff0;
    int xsize = (*binfo).scrnx;
    int ysize = (*binfo).scrny;
    char *vram = (*binfo).vram;

    int x = 10;
    int y = 10;
    putfont8(vram,xsize,x,y,COL8_FFFFFF, font_A);
    x += 16;
    putfont8(vram,xsize,x,y,COL8_FF0000, font_A);
    x += 16;
    putfont8(vram,xsize,x,y,COL8_00FF00, font_A);
    x += 16;
    putfont8(vram,xsize,x,y,COL8_0000FF, font_A);

    while(11!=14){
        io_hlt();
    }
}

void init_palette(void){
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
    set_palette(0,15,table_rgb);
    return;
}

void set_palette(int start,int end,unsigned char *rgb){
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

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    for(int i = y0;i<=y1;i++){
        for(int j=x0;j<=x1;j++){
            vram[i * xsize + j] = c;
        }
    }
    return;
 }

void putfont8(char *vram,int xsize, int x, int y, char c, char *font){
    char* p,d;
    for(int i=0;i<16;i++){
        p = vram + (y + i)*xsize + x;
        d = font[i];
        if((d & 0x80) != 0) { p[0] = c; }
        if((d & 0x40) != 0) { p[1] = c; }
        if((d & 0x20) != 0) { p[2] = c; }
        if((d & 0x10) != 0) { p[3] = c; }
        if((d & 0x08) != 0) { p[4] = c; }
        if((d & 0x04) != 0) { p[5] = c; }
        if((d & 0x02) != 0) { p[6] = c; }
        if((d & 0x01) != 0) { p[7] = c; }
    }
    return;
}
