#include "kprintf.h"
#include "console.h"

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
            set_pixel(x, y, ( ( (x+y) & 1)?black:white ) );

    kprintf("\nDONE\n");
    while(1){
    }
}