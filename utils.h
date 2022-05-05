#ifndef H_UTILS
#define H_UTILS

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* str_escape(const char* str)
{
    char* c_ptr = (char*)str;
    char* buffer = (char*)calloc(strlen(str) * 5 + 1, sizeof(char));
    char* eb_ptr = (char*)buffer;

    while (*c_ptr != 0) {
        if (*c_ptr > 31 && *c_ptr < 127) {
            *eb_ptr = *c_ptr;
            eb_ptr += 1;
        } else {
            sprintf(eb_ptr, "\\x%02x", (int)*c_ptr);
            eb_ptr += 4;
        }
        ++c_ptr;
    }
    *eb_ptr = 0;

    return buffer;
}

static bool is_numeric_string(const char* str)
{
    char* itr = (char*)str;

    if (*itr == 0) {
        return false;
    }

    while (*itr != 0) {
        if (!isdigit(*itr))
            return false;
        ++itr;
    }

    return true;
}

#endif