#ifndef H_FNV
#define H_FNV

/**
 * @brief FNV1A Hash
 * 
 * Simplified and translated from JavaScript implementation
 * (https://github.com/sindresorhus/fnv1a/blob/main/index.js)
 */

#include <stdint.h>

#define FNV_PRIME_64 1099511628211ULL
#define FNV_OFFSET_64 14695981039346656037ULL

uint64_t fnv1a(const char* string);

#endif