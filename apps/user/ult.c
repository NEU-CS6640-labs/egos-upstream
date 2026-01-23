/*
 * (C) 2023, Cornell University
 * All rights reserved.
 */

/* Author: Robbert van Renesse
 * Description: queue and user-level threading
 * Students implement a queue and a threading package;
 * And then spawn multiple threads as either producer or consumer.
 *
 * Updated by CS6640 23fall and 26spring staff
 */

#include "app.h"
#include "queue.h"

/** These two functions are defined in app/user/thread.s **/
void ctx_start(void** old_sp, void* new_sp);
void ctx_switch(void** old_sp, void* new_sp);

/** Multi-threading functions **/
void thread_create();
void thread_yield();
void thread_exit();


struct thread {
    /* [lab2-ex1]
     * TODO: your code here
     */

};


void thread_init(){
    /* [lab2-ex4]
     * TODO: your code here
     */

    INFO("thread_init is not implemented\n");
    exit(0);
}


void thread_create(void (*f)(void *), void *arg, unsigned int stack_size) {
    /* [lab2-ex4]
     * TODO: your code here
     * note: stack grows from HIGH to LOW in memory
     */

    INFO("thread_create is not implemented\n");
    exit(0);
}



void ctx_entry(void){
    /* [lab2-ex4]
     * TODO: your code here
     */

    INFO("ctx_entry is not implemented\n");
    exit(0);
}

void thread_yield(){
    /* [lab2-ex5]
     * TODO: your code here
     */

    INFO("thread_yield is not implemented\n");
    exit(0);
}

void thread_exit(){
    /* [lab2-ex5]
     * TODO: your code here
     */

    INFO("thread_exit is not implemented\n");
    exit(0);
}

/* ===========================
 * test cases from ult_test.c
 * ===========================
 * */
void test_create();
void test_stack();
void test_exit();
void test_single_yield();
void test_pingpong_yield();
void test_producer_consumer();


int main(int args, void **argv) {

    if (args != 2) {
        printf("Usage: ult <test: a-f>\n");
        exit(1);
    }

    switch ( ((char**)argv)[1][0]) {
        /* test cases for [lab2-ex4]
         * These tests touch thread_init, thread_create, and ctx_entry.
         * You should run one test at a time.
         */
        case 'a':
            test_create();
            break;
        case 'b':
            test_stack();
            break;
        /* test case for [lab2-ex5]
         * These tests touch all functions.
         * You should run one test at a time.
         */
        case 'c':
            test_exit();
            break;
        case 'd':
            test_single_yield();
            break;
        case 'e':
            test_pingpong_yield();
            break;
        case 'f':
            test_producer_consumer();
            break;
        default:
            printf("test-%s is unknown. Choose from test-[a..f]\n", argv[1]);
    }

    return 0;
}

