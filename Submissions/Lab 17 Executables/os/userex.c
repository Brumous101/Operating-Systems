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

int main(int argc, char* argv[])
{
    void *p = print;
    print("Hello from userland!\nI am at address ");
    print_hex((unsigned)p);
    print("\n");
    
    while(1){
    }
    
    return 0;
}
