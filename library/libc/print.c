/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: formatted printing
 * format_to_str() converts a format into a C string:
 * e.g., converts ("%s-%d%s", "egos", 2, "k+") to "egos-2k+".
 * term_write() prints the converted C string to the screen.
 */

#include "egos.h"
#include "servers.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void format_to_str(char* out, const char* fmt, va_list args) {
    for (out[0] = 0; *fmt != '\0'; fmt++) {
        if (*fmt != '%') {
            strncat(out, fmt, 1);
        } else {
            fmt++;
            if (*fmt == 's') {
                strcat(out, va_arg(args, char*));
            } else if (*fmt == 'd') {
                itoa(va_arg(args, int), out + strlen(out), 10);
            } else if (*fmt == 'x') {
                itoa(va_arg(args, int), out + strlen(out), 16);
            } else if (*fmt == '%') {   // print '%'
                size_t len = strlen(out);
                out[len] = '%';
                out[len+1] = '\0';
            } else if (*fmt == 'f') {
                double v = va_arg(args, double);

                /* handle sign */
                if (v < 0) {
                    size_t len = strlen(out);
                    out[len] = '-';
                    out[len + 1] = '\0';
                    v = -v;
                }

                /* scale and round to two decimals */
                unsigned long long scaled = (unsigned long long)(v * 100.0 + 0.5);
                unsigned long long ipart  = scaled / 100;
                unsigned long long fpart  = scaled % 100;
                /* integer part */
                itoa((int)ipart, out + strlen(out), 10);
                /* decimal point */
                strcat(out, ".");
                /* fractional part: always two digits */
                size_t len = strlen(out);
                out[len]     = '0' + (fpart / 10);
                out[len + 1] = '0' + (fpart % 10);
                out[len + 2] = '\0';
            }

            /* [lab1-ex7]
             * TODO:
             * - handle format %c, %u, %p, %lld, %llu and %llx.
             * - if you don't know, google the meaning of format like '%u'
             * Hints:
             *   - read '$ man va_arg' to understand how to fetch the arguments
             *   - for numbers, you can either implement something like `itoa`
             *     or using other functions in standard C library
             *     or search for utility functions like `itoa`
             * */
            else {
                char tmp[2];
                tmp[0] = *fmt;
                tmp[1] = '\0';
                FATAL("printf cannot parse the format: '%%%s'", tmp);
            }

        }
    }
}

#define LOG(prefix, suffix)                                                    \
    char buf[512];                                                             \
    strcpy(buf, prefix);                                                       \
    va_list args;                                                              \
    va_start(args, format);                                                    \
    format_to_str(buf + strlen(prefix), format, args);                         \
    va_end(args);                                                              \
    strcat(buf, suffix);                                                       \
    term_write(buf, strlen(buf));

int my_printf(const char* format, ...) { LOG("", ""); }

int INFO(const char* format, ...) { LOG("[INFO] ", "\n\r") }

int FATAL(const char* format, ...) {
    LOG("\x1B[1;31m[FATAL] ", "\x1B[1;0m\n\r") /* \x1B[1;31m means red. */
    while (1);
}

int SUCCESS(const char* format, ...) {
    LOG("\x1B[1;32m[SUCCESS] ", "\x1B[1;0m\n\r") /* \x1B[1;32m means green. */
}

int CRITICAL(const char* format, ...) {
    LOG("\x1B[1;33m[CRITICAL] ", "\x1B[1;0m\n\r") /* \x1B[1;33m means yellow. */
}
