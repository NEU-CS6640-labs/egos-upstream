/*
 * a program corrupting the memory of earth, disk, and free memory.
 */

#include "app.h"
#include <string.h>

void tamper_proc() {
    memset((void*)(APPS_PAGES_BASE), 0, PAGE_SIZE*10);
    SUCCESS("Crash3 succeeds in corrupting the memory of other processes");
}

void tamper_earth() {
    /* modifying earth's code */
    int *tmp = (int*) EARTH_STRUCT;
    *tmp = 0;
    SUCCESS("Crash3 succeeds in modifying earth's code");
}

void tamper_disk() {
    /* modifying disk content*/
    int *tmp = (int*) FLASH_ROM_BASE;
    *tmp = 0;
    SUCCESS("Crash3 succeeds in modifying disk contents");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        tamper_earth();
        tamper_disk();
        tamper_proc();
    } else if (strcmp(argv[1], "disk") == 0) {
        tamper_disk();
    } else if (strcmp(argv[1], "earth") == 0) {
        tamper_earth();
    } else if (strcmp(argv[1], "proc") == 0) {
        tamper_proc();
    } else {
        printf("Usage: crash3 [proc|disk|earth]\n");
    }
}
