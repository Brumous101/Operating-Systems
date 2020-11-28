void kmemcpy(void* dp, const void *sp, unsigned count)
{
    char* d = (char*) dp;
    char* s = (char*) sp;
    for(unsigned i=0; i<count; ++i)
        *d++ = *s++;
}

void halt(){
    while(1)
        ;
}