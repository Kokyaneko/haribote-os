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
void init_screen(char *vram, int xsize, int ysize);

void putfont8(char *vram,int xsize, int x, int y, char c, char *font);
void putfont8_asc(char *vram,int xsize, int x, int y, char c, char *s);
extern char hankaku[4096];

void init_mouse_cursor8(char *mouse,char bc);
void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize);

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    char *vram;
};

void main(void){
    init_palette();
    struct BOOTINFO *binfo;
    binfo = (struct BOOTINFO *) 0x0ff0;

    init_screen(binfo->vram,binfo->scrnx,binfo->scrny);
    putfont8_asc(binfo->vram,binfo->scrnx,8,8,COL8_FFFFFF, "ABC 123" );
    putfont8_asc(binfo->vram,binfo->scrnx,30,30,COL8_000000, "Haribote OS" );
    putfont8_asc(binfo->vram,binfo->scrnx,31,31,COL8_FFFFFF, "Haribote OS" );//make string 3D

    char mcursor[256];
    init_mouse_cursor8(mcursor,COL8_008484);
    int mx = (binfo->scrnx)/2;
    int my = (binfo->scrny)/2;
    putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);

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

void init_screen(char *vram, int xsize, int ysize){
    boxfill8(vram, xsize, COL8_008484, 0, 0, xsize - 1, ysize - 29);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
    boxfill8(vram, xsize, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

    boxfill8(vram, xsize, COL8_FFFFFF, 3, ysize - 24, 59, ysize - 24);
    boxfill8(vram, xsize, COL8_FFFFFF, 2, ysize - 24, 2, ysize - 4);
    boxfill8(vram, xsize, COL8_848484, 3, ysize - 4, 59, ysize - 4);
    boxfill8(vram, xsize, COL8_848484, 59, ysize - 23, 59, ysize - 5);
    boxfill8(vram, xsize, COL8_000000, 2, ysize - 3, 59, ysize - 3);
    boxfill8(vram, xsize, COL8_000000, 60, ysize - 24, 60, ysize - 3);

    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize - 4, ysize - 24);
    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize - 4);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize - 3, xsize - 4, ysize - 3);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 3, ysize - 24, xsize - 3, ysize - 3);
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

void putfont8_asc(char *vram,int xsize, int x, int y, char c, char *s){
    extern char hankaku[4096];
    for(;*s != 0x00;s++){
        putfont8(vram,xsize,x,y,c,hankaku+ *s *16);
        x += 8;
    }
    return;
}

void init_mouse_cursor8(char *mouse,char bc){
    static char cursor[16][16] = {
        "************....",
        "*OOOOOOOOOO**...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "***......*OOO*..",
        ".*........*OOO*.",
        "...........*OOO*",
        "............*OO*",
        ".............***"
    };
    for(int i=0;i<16;i++){
        for(int j=0;j<16;j++){
            if(cursor[i][j] == '*') mouse[i*16+j] = COL8_000000;
            if(cursor[i][j] == 'O') mouse[i*16+j] = COL8_FFFFFF;
            if(cursor[i][j] == '.') mouse[i*16+j] = bc;
        }
    }
    return;
}

void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize){
    for(int i=0;i<pysize;i++){
        for(int j=0;j<pxsize;j++) vram[(py0+i)*vxsize+(px0+j)] = buf[i*bxsize + j];
    }
    return;
}
