//naskfunc.asmとリンク
//BOOT情報を取得

#include "set.h"

void init_mouse_cursor8(char *mouse,char bc);
void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize);

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

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

void init_mouse_cursor8(char *mouse,char bc){
    static char cursor[16][16] = {
        "************....",
        "*OOOOOOOOOO*....",
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

void init_gdtidt(void){
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x00270000;

    // init GDT
    for(int i=0;i<8192;i++){
        set_segmdesc(gdt + i,0,0,0);
    }
    set_segmdesc(gdt + 1,0xffffffff,0x00000000,0x4092);
    set_segmdesc(gdt + 2,0x0007ffff,0x00280000,0x409a);
    load_idtr(0x7ff,0x00270000);

    // init IDT
    for(int i=0;i<256;i++){
        set_gatedesc(idt+i,0,0,0);
    }
    load_idtr(0x7ff, 0x26f800);
    return;
}
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
    if(limit > 0xfffffa){
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low   = limit & 0xffff;
    sd->base_low    = base & 0xffff;
    sd->base_mid        = (base >> 16) & 0xff;
    sd->access_right    = ar & 0xff;
    sd->limit_high      = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high       = (base >> 24) & 0xff;
    return;
}
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
    gd->offset_low      = offset & 0xffff;
    gd->selector        = selector;
    gd->dw_count        = (ar >> 8) & 0xff;
    gd->access_right    = ar & 0xff;
    gd->offset_high     = (offset >> 16) & 0xffff;
    return;
}
