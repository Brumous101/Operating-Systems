#include "kprintf.h"
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
    kprintf(declaration);

    while(1){
    }
}