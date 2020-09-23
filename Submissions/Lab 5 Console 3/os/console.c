#include "console.h"
#include "kprintf.h"

#define pl110 ( (volatile unsigned*)0xc0000000 )
#define haxis       (pl110+0)    //horizontal axis
#define vaxis       (pl110+1)    //vertical axis
#define polarity    (pl110+2)    //clock+polarity
#define lineend     (pl110+3)    //line end
#define baseaddr1   (pl110+4)    //upper panel base address
#define baseaddr2   (pl110+5)    //lower panel base address
#define intmask     (pl110+6)    //interrupt mask
#define params      (pl110+7)    //panel parameters
#define framebuffer ((volatile unsigned short*) (((0x07ffffff - WIDTH * HEIGHT * 2)) & ~0xf))

static volatile unsigned* serialport = (unsigned*) 0x16000000;
static volatile unsigned* serialflags = (unsigned*) 0x16000018;

void serial_outchar(char c){
    while(*serialflags & (1<<5) ){
    }
    //No longer idle, can do stuff
    *serialport=c;
}
//console_putc is called in kprintf
void console_putc(char c){
    serial_outchar(c);
    //More stuff eventually
}

void console_init(){
    *haxis = 0x3f1f3f00 | (WIDTH/4-4);
    *vaxis = 0x80b6000 | (HEIGHT-1);
    *polarity = 0x067f3800;
    *params = 0x1820 | 9;
}

void setPixel( int x, int y, unsigned short color){
    framebuffer[x+(y*WIDTH)] = color;
    *baseaddr1 = (unsigned)framebuffer;
}

void draw_rectangle( int x, int y, int width, int height, unsigned short color){
    for(int j=0; j<height;j++){
        for(int i = x+(WIDTH*(y+j)); i<= (x+(WIDTH*(y+j))+width); i++){
            framebuffer[i] = color;
            *baseaddr1 = (unsigned)framebuffer;
        }
    }
}

char get_key(){
    while(1){
        unsigned fl = *serialflags;
        if( fl & 0x40 ){
            unsigned x = *serialport;
            return x;
        }
    }
}