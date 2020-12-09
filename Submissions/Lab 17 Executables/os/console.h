//console.h
#pragma once
//(console.h) these defines are available outside of the main function when we import this header!
#define HEIGHT 600
#define WIDTH 800

void serial_outchar(char c);
//Not in notes but I added this here:
void console_putc(char c);
void console_init();
void setPixel( int x, int y, unsigned short color);
void draw_rectangle( int x, int y, int width, int height, unsigned short color);
char get_key();
void console_clear_screen();
void gulp();
void check_column_length();
void check_row_length();
void scroll();