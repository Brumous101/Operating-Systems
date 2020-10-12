#include "console.h"
#include "kprintf.h"
#include "font.h"
#include "utility.h"

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

//char cell: x=0...79
//char cell: y=0... 37?
struct Position{
    int col; //x
    int row; //y
};

static struct Position cursor;
static volatile unsigned* serialport = (unsigned*) 0x16000000;
static volatile unsigned* serialflags = (unsigned*) 0x16000018;
int escflag = 0;

void serial_outchar(char c){
    while(*serialflags & (1<<5) ){
    }
    //No longer idle, can do stuff
    *serialport=c;
}
//console_putc is called in kprintf
void console_putc(char c){
    serial_outchar(c);
    if(escflag == 0){
        switch(c)
        {
            case 10://Newline '\n'
                cursor.row += 1;
            case 13://Carriage return '\r'
                cursor.col = 0;
                check_column_length();
                break;
            case 9://Tab '\t'
                cursor.col += 8-(cursor.col%8);
                check_column_length();
                break;
            case 12://Form feed '\f'
                //Clear the screen
                cursor.col = 0;
                cursor.row = 0;
                check_column_length();
                //draw framebuffer all black
                console_clear_screen();
                break;
                
            case 27://Escape '\e'
                //gulp();
                escflag = 1;
                break;
            case 127://Backspace (rubout) ''
                if(cursor.col == 0 && cursor.row == 0){
                    drawCharacter(' ', 1*CHAR_WIDTH, 1*CHAR_HEIGHT);
                }
                else if(cursor.col == 0){
                    cursor.row--;
                    cursor.col = 79;
                    drawCharacter(' ', cursor.col*CHAR_WIDTH, cursor.row*CHAR_HEIGHT);
                }
                else{
                    cursor.col--;
                    drawCharacter(' ', cursor.col*CHAR_WIDTH, cursor.row*CHAR_HEIGHT);
                }
                check_column_length();
                break;
            default:
                drawCharacter(c, cursor.col*CHAR_WIDTH, cursor.row*CHAR_HEIGHT);
                cursor.col++;
                check_column_length();
                break;
        }
    }
    else{
        escflag = 0;
    }
}

void check_column_length(){
    if(cursor.col > 79){
        cursor.col = 0;
        cursor.row++;
    }
    check_row_length();
}

void check_row_length(){
    if(cursor.row > 36){
        scroll();
        cursor.row = 36;
    }
}

void scroll(){
    //first row becomes second row
    //dest, src, size in bytes
    kmemcpy((void*)framebuffer, (void*)(framebuffer + (WIDTH * CHAR_HEIGHT)), ((WIDTH * HEIGHT * 2) - (WIDTH * CHAR_HEIGHT * 2)));
    //clear last row on screen
    draw_rectangle( 0, HEIGHT-CHAR_HEIGHT, WIDTH, CHAR_HEIGHT, 0b1010100000000000);
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

void console_clear_screen()
{
    draw_rectangle(0, 0, WIDTH, HEIGHT, 0b1010100000000000 );
}

void gulp()
{
    kprintf("gulp");
}