#include "kprintf.h"
#include "console.h"
#include "font.h"

extern char sbss;
extern char ebss;

char* declaration ="When in the Course of human events, it becomes necessary for one\n"
"people to dissolve the political bands which have connected\n"
"them with another, and to assume among the powers of the\n"
"earth, the separate and equal station to which the Laws of\n"
"Nature and of Nature's God entitle them, a decent respect to\n"
"the opinions of mankind requires that they should declare the\n"
"causes which impel them to the separation.";

void clearBss(){
    char* start = &sbss;
    char* end = &ebss;
    char* p = start;
    while(p != end){
        *p=0;
        ++p;
    }
}


void kmain(){
    clearBss();
    console_init();
    kprintf(declaration);

    unsigned short black = 0x0000;
    unsigned short white = 0xffff;

    for(int y=0; y < HEIGHT; y++)
        for(int x=0; x < WIDTH; x++)
            setPixel(x, y, ( ( (x+y) & 1)?black:white ) );

    //This is the location for the rectangle. Trying to make it reuseable
    //draw_rectangle(((WIDTH-32)>>1), ((HEIGHT-32)>>1), 32, 32, black);
    
    drawCharacter( 'O', 100, 200);
    drawCharacter( 'S', 110, 200);
    drawCharacter( '1', 120, 200);
    
    //unsigned red=0,green=0,blue=0, fivebitmask= 0b11111, sixbitmask= 0b111111;
    //unsigned short rgb;
    kprintf("\nDONE\n");
    while(1){/*
        char c = get_key();
        if(c == 'q' && red < 31){
            red++;
        }
        else if(c == 'a' && red != 0){
            red--;
        }
        else if(c == 'w' && green < 63){
            green++;
        }
        else if(c == 's' && green != 0){
            green--;
        }
        else if(c == 'e' && blue < 31){
            blue++;
        }
        else if(c == 'd' && blue != 0){
            blue--;
        }
        else if(c =='i'){
            blue = blue ^ fivebitmask;
            green = green ^ sixbitmask;
            red = red ^ fivebitmask;
        }
        rgb = ( ( ( (blue << 6) | (green) ) << 5 ) | red);
        kprintf("\nrgb: %d", rgb);
        draw_rectangle(((WIDTH-32)>>1), ((HEIGHT-32)>>1), 32, 32, rgb);
        */
    }
}