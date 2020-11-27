#include "console.h"
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