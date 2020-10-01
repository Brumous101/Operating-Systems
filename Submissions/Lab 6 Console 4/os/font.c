#include "font.h"
#include "console.h"
//draw character ch starting at (x,y)
void drawCharacter( int ch, int x, int y){
    int r,c;
    for(r=0;r<CHAR_HEIGHT;++r){
        for(c=0;c<CHAR_WIDTH;++c){
            if( font_data[ch][r] & (MASK_VALUE >>c) )
                setPixel( x+c, y+r, 0xffff );
            else
                setPixel( x+c, y+r, 0x0000 );
        }
    }
}