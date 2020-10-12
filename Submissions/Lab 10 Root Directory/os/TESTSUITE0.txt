#include "kprintf.h"

const char* bigstring = 
    "\fWhen in the course of human. No."
    "We the people of. Oh crud."
    "|       |       |       |       |       |       |       |       |       |       |       |       |\n"
    "Fourscore and\t^seven years ago, our\t^forefathers brought\t^forth upon\t^this continent\n"
    "a new \t^NATION conceived in libibertariany and\t\t\t^dedicated to\t\t^the"
    "\t\t^NOTION\t^that\n"
    "\n"
    "all menpeople are created equal.\n"
    "\n"
    "\n"
    "Now\n"
    "\twe\n\t\tare\n\t\t\tengaged.................,..............in..............,.............a............,..........large,"
    "yooge, egregious, massive, overbearing grate""eat civil strife,\n"
    "~~~~~~~\t:-)\n"
    "\t\t\t\t\ttesting whether that "
    "nation or fooby"
    "any" "any "
    "nation can so endure."
    ;
    

void sweet(){
    int i;
    const char* p = bigstring;
    while(*p){
        kprintf("%c",*p++);
    }
    kprintf("\n");
    for(i=0;i<201;++i)
        kprintf("$");
    kprintf("\n");
    kprintf("A");
    kprintf("B");
    kprintf("C");
    kprintf("D");
    kprintf("D");
    kprintf("D");
    kprintf("\n");
    for(i=0;i<20;i++)
        kprintf("*");
    kprintf("!");
    kprintf("#");
}



