#include "app.h"

void ptr_is_address();
void ins_is_zero_one();
void impl_printf();

int main(int args, char **argv) {
    /* [lab1-ex3]
     * TODO: print hello world
     */


    /* [lab1-ex7]
     * TODO:
     * - implement format_to_str(...) in libc/print.c
     * - uncomment and run impl_printf()
     */
    // impl_printf();
}

void ptr_is_address() {
    /* [lab1-ex5]
     * TODO:
     * - replace 0x0 with a __valid__ memory address
     * - call ptr_is_address() in main()
     */
    int *ptr = (int *) 0x0;

    // read value from the ptr
    int val = *ptr;

    // write value to the ptr
    *ptr = val + 1;

    printf("ptr:%x before:%d after:%d\n", ptr, val, *ptr);
}

void ins_is_zero_one() {
    printf("Enter ins_is_zero_one()\n");

    // the instruction address where app exits
    int *exit_code = (int*) 0x8020000C;

    /* [lab1-ex6]
     * TODO:
     * - replace 0x0 with the __binary encoding__ of the `call main` instruction.
     * - determine the encoding by checking `build/debug/helloworld.lst`
     * - locate the instruction labeled `call main`.
     * - call ins_is_zero_one() in main()
     */
    int call_main = 0x0;

    // replace with
    *exit_code = call_main;

    while(1);
}


void impl_printf() {
    void *msg = "hello world";
    printf("%c is character $\n", '$');
    printf("%c is character 0\n", (char)48);
    printf("%u is the maximum of unsigned int\n", (unsigned int)0xFFFFFFFF);
    printf("%p is the hexadecimal address of the hello-world string\n", msg);
    printf("%lld is the maximum of long long\n", 0x7FFFFFFFFFFFFFFFULL);
    printf("%llu is the maximum of unsigned long long\n", 0xFFFFFFFFFFFFFFFFULL);
    printf("%llx is the maximum of unsigned long long in hex\n", 0xFFFFFFFFFFFFFFFFULL);
}


