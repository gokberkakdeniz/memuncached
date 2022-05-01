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

#include <stdbool.h>
#include <sys/types.h>

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
typedef u_char cache_value_type;
typedef char cache_value_key[256];

/**
 * @brief cache table value
 */
typedef struct cache_value_t {
    cache_value_key key;
    cache_value_type type;
    void* value;
} cache_value;

/**
 * @brief cache table instance
 */
typedef struct cache_table_t {
    cache_value* table;
    int64_t count;
} cache_table;

/**
 * @brief constructor.
 * 
 * @param size table size
 * @return cache_table* instance
 */
cache_table* cache_table_create(int64_t size);

/**
 * @brief destructor.
 * 
 * @param table instance
 */
void cache_table_destroy(cache_table* table);

/**
 * @brief gets value from table. if not found returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value* value 
 */
cache_value* cache_table_get(cache_table* table, cache_value_key key);

/**
 * @brief puts/replaces new value to table.
 * 
 * @param table instance
 * @param key cache key
 * @param type cached value type
 * @param value cache value
 * @return bool
 */
bool cache_table_set(cache_table* table, cache_value_key key, cache_value_type type, void* value);

/**
 * @brief increments cache value. if not found or not number, returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value* 
 */
cache_value* cache_table_inc(cache_table* table, cache_value_key key);

/**
 * @brief decrements cache value. if not found or not number, returns NULL.
 * 
 * @param table instance
 * @param key cache key
 * @return cache_value* 
 */
cache_value* cache_table_dec(cache_table* table, cache_value_key key);

/**
 * @brief deletes cache value if found.
 * 
 * @param table instance
 * @param key cache value
 * @return bool
 */
bool cache_table_del(cache_table* table, cache_value_key key);

#endif