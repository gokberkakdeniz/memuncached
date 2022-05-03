#include "fnv.h"

uint64_t fnv1a(const char* string)
{
    uint64_t hash = FNV_OFFSET_64;
    char* cptr;

    for (cptr = (char*)string; *cptr != 0; cptr++) {
        hash = (hash ^ *cptr) * FNV_PRIME_64;
    }

    return hash;
}
