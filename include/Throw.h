//This is a simple throw/catch API that help with safely handling errors.
//Works similar to errno. Uses a global error variable to set errors.

#pragma once

//Throw error. Followed by default function return value (usually NULL).
#define throw(msg) \
    fputs(msg, stderr); putchar('\n'); err = true; return

//Throw error with formatted message. Followed by default function return value (usually NULL).
#define throwf(msg, ...) \
    fprintf(stderr, msg, ## __VA_ARGS__); err = true; return

//Simple macro to contextualize handling.
#define catch \
    if (err)

//Memcpy callback to keep structure of visitor() pretty.
#define _memcpy(dest, src, size) \
    sw = src; \
    catch { \
        return 0; \
    } else memcpy(dest, sw, size);

//Global error value.
extern _Bool err;