extern unsigned sbss,ebss;
extern int main();

int _start(){
    //clear the bss
    char* p = (char*)&sbss;
    while(p != (char*)&ebss){
        *p=0;
        p++;
    }
    return main();
}