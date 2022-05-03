#ifndef H_HASH_TABLE
#define H_HASH_TABLE

/**
 * @file cache_table.h
 * @author Gokberk Akdeniz
 * @brief Hash table for caching decimal, real and string values
 * @version 0.1
 * @date 2022-05-01
 * 
 * @copyright Copyright (c) 2022
 */

#include "fnv.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Cache table value types
 */
#define CACHE_VALUE_DECIMAL 0
#define CACHE_VALUE_REAL 1
#define CACHE_VALUE_STRING 2
typedef int64_t cache_value_decimal;
typedef double cache_value_real;
typedef char* cache_value_string;

/**
 * Cache table value subtypes
 */
typedef unsigned char cache_value_type;
typedef char* cache_value_key;

/**
 * @brief cache table value
 */
typedef struct cache_value {
    cache_value_key key;
    cache_value_type type;
    void* value;
} cache_value_t;

/**
 * @brief cache table instance
 */
typedef struct cache_table {
    cache_value_t** table;
    uint64_t count;
    uint64_t size;
} cache_table_t;

/**
 * @brief constructor.
 * 
 * @param size table size
 * @return cache_table_t* instance
 */
cache_table_t* cache_table_create(uint64_t size);

/**
 * @brief destructor.
 * 
 * @param table instance
 */
void cache_table_destroy(cache_table_t* table);

/**
 * @brief gets value from table. if not found returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value_t* value 
 */
cache_value_t* cache_table_get(cache_table_t* table, cache_value_key key);

/**
 * @brief puts/replaces new value to table.
 * 
 * @param table instance
 * @param key cache key
 * @param type cached value type
 * @param value cache value
 * @return bool
 */
bool cache_table_set(cache_table_t* table, cache_value_key key, cache_value_type type, void* value);

/**
 * @brief increments cache value. if not found or not number, returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value_t* 
 */
cache_value_t* cache_table_inc(cache_table_t* table, cache_value_key key);

/**
 * @brief decrements cache value. if not found or not number, returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value_t* 
 */
cache_value_t* cache_table_dec(cache_table_t* table, cache_value_key key);

/**
 * @brief deletes cache value if found.
 * 
 * @param table instance
 * @param key cache value
 * @return bool
 */
bool cache_table_del(cache_table_t* table, cache_value_key key);

#endif