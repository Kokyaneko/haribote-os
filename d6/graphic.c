#include "set.h"

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

void putfont8_asc(char *vram,int xsize, int x, int y, char c, char *s){
    extern char hankaku[4096];
    for(;*s != 0x00;s++){
        putfont8(vram,xsize,x,y,c,hankaku+ *s *16);
        x += 8;
    }
    return;
}
