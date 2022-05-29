#ifndef H_UTILS
#define H_UTILS

#include <ctype.h>
#include <stdbool.h>
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

static bool is_decimal_string(const char* str)
{
    char* itr = (char*)str;

    if (*itr == 0) {
        return false;
    }

    if (*itr == '+' || *itr == '-') {
        itr++;
    }

    while (*itr != 0) {
        if (!isdigit(*itr))
            return false;

        ++itr;
    }

    return true;
}

static bool is_real_string(const char* str)
{
    char* itr = (char*)str;

    if (*itr == 0 || *itr == '.') {
        return false;
    }

    if (*itr == '+' || *itr == '-') {
        itr++;
    }

    bool dot = false;

    while (*itr != 0) {
        if (*itr == '.') {
            if (dot) {
                return false;
            }

            dot = true;
        } else if (!isdigit(*itr)) {
            return false;
        }

        ++itr;
    }

    return true;
}

// static inline bool is_positive_string(const char* str)
// {
//     return *str != NULL && *str != '-';
// }

#endif