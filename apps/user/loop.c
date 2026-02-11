/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a simple loop
 */

#include "app.h"
#include <stdlib.h>

void loop(int count) {
    int counter = 0;
    for(int i=0;i<count*QUANTUM; i++) {
        float ret = (float)i / counter;
        counter++;
    }
}

void sleep(int time) {
    sys_sleep(time);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        loop(100);
    } else if (strcmp(argv[1], "sleep") == 0) {
        printf("Sleep for 5 QUANTUM...\n");
        sleep(5);
        printf("                   ...and waked up\n");
    }
}
