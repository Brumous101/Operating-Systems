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