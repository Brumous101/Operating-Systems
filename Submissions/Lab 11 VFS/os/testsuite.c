#include "errno.h"
#include "kprintf.h"

extern int file_open(const char* fname, int flags);
extern int file_close(int fd);

void sweet(){
    int i;
    char tmp[32];
    int fd;

    const char* nonexist[] = {
        "blargh", "blargleblargleboom", 
        "blargleblargleboom.burpblah", "b.burpblah",
        "b.b", "art.txt","article1234.txt",
        "article 2.TXT",
        "article1.T","article1txt",0};
    
    kprintf("Testing nonexistent file open...\n");
    for(i=0; nonexist[i] ; ++i){
        fd = file_open( nonexist[i], 0);
        if( fd >= 0 ){
            kprintf("Opened nonexistent file %s\n",nonexist[i]);
            return;
        }
    }

    kprintf("Testing closing nonopen...\n");
    for(i=-4;i<10;++i){
        if( file_close(i) == 0 ){
            kprintf("Closed nonopen %d\n",i);
            return;
        }
    }
    
    kprintf("Testing open...\n");
    int used[6];
    for(i=0;i<6;++i)
        used[i] = -1;
    int j;
    for(i=0;i<=6;++i){
        if( i == 0 )
            ksprintf(tmp,"BILL.TXT");
        else {
            if( i % 2 == 0 )
                ksprintf(tmp,"ARTICLE%d.TXT",i);
            else
                ksprintf(tmp,"article%d.txt",i);
        }
        
        fd = file_open(tmp,0);
        if(fd < 0 ){
            kprintf("Could not open %s\n",tmp);
            return;
        }
        for(j=0;j<6;++j){
            if( used[j] == fd ){
                kprintf("Duplicate fd\n");
                return;
            }
        }
        used[i-1]=fd;
    }

    for(i=0;i<6;++i){
        if( file_close(used[i]) != SUCCESS ){
            kprintf("Could not close %d\n",used[i]);
            return ;
        }
    }
    
    kprintf("Exhaustion test...\n");

    //exhaustion test
    for(i=0;i<1000;++i){
        file_open("ARTICLE1.txt",0);
    }
    fd = file_open("ARTICLE1.txt",0);
    if(fd >= 0 ){
        kprintf("Exhaustion test failed\n");
        return;
    }
        
    kprintf("All OK\n");
}
