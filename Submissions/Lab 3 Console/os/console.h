//console.h
#pragma once
//(console.h) these defines are available outside of the main function when we import this header!
#define HEIGHT 600
#define WIDTH 800

void serial_outchar(char c);
//Not in notes but I added this here:
void console_putc(char c);
void console_init();
void set_pixel( int x, int y, unsigned short color);