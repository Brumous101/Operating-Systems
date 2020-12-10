#include "syscalldefs.h"

static volatile unsigned* serialport = (unsigned*) 0x16000000;
static volatile unsigned* serialflags = (unsigned*) 0x16000018;

void serial_outchar(char c){
    while( *serialflags & (1<<5) )
        ;
    *serialport = c;
}

void print(const char* p){
    while(*p){
        serial_outchar(*p);
        p++;
    }
}

void print_hex(unsigned x){
    print("0x");
    for(int i=28;i>=0;i-=4){
        int v = (x>>i) & 0xf;
        if( v >= 0 && v <= 9 )
            serial_outchar( '0'+v );
        else
            serial_outchar( 'a' + (v-10) );
    }
}

int make_syscall(unsigned req, unsigned p1, unsigned p2, unsigned p3){
    register unsigned tmp0 asm ("r0");
    register unsigned tmp1 asm ("r1");
    register unsigned tmp2 asm ("r2");
    register unsigned tmp3 asm ("r3");
    tmp0=req;
    tmp1=p1;
    tmp2=p2;
    tmp3=p3;
    asm volatile(
          "svc #42"
        : "+r"(tmp0)
        : "r"(tmp1), "r"(tmp2), "r"(tmp3)
        : "memory","cc");
    return tmp0;
}

int main(int argc, char* argv[])
{
    make_syscall(SYSCALL_TEST,0,0,0);
    make_syscall(SYSCALL_TEST,10,20,30);
    while(1){
    }
    return 0;
}