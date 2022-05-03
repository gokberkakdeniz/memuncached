#include "cache_table.h"
#include <stdio.h>

cache_table_t* cache_table_create(uint64_t size)
{
    cache_table_t* table_ptr = (cache_table_t*)malloc(sizeof(cache_table_t));

    table_ptr->table = (cache_value_t**)calloc(size, sizeof(cache_value_t*));
    table_ptr->size = size;
    table_ptr->count = 0;

    return table_ptr;
}

void cache_table_destroy(cache_table_t* table)
{
    for (size_t i = 0; i < table->size; i++) {
        if (table->table[i] != NULL) {
            free(table->table[i]->value);
            free(table->table[i]);
        }
    }

    free(table->table);
    free(table);
}

bool cache_table_set(cache_table_t* table, cache_value_key key, cache_value_type type, void* value)
{
    int64_t index = fnv1a(key) % table->size;

    if (table->table[index] == NULL) {
        table->table[index] = (cache_value_t*)malloc(sizeof(cache_value_t));
    }

    table->table[index]->key = key;
    table->table[index]->type = type;

    if (type == CACHE_VALUE_DECIMAL) {
        table->table[index]->value = (cache_value_decimal*)malloc(sizeof(cache_value_decimal));
        *(cache_value_decimal*)table->table[index]->value = *(cache_value_decimal*)value;
    } else if (type == CACHE_VALUE_REAL) {
        table->table[index]->value = (cache_value_real*)malloc(sizeof(cache_value_real));
        *(cache_value_real*)table->table[index]->value = *(cache_value_real*)value;
    } else if (type == CACHE_VALUE_STRING) {
        size_t size = strlen(*(cache_value_string*)value) * sizeof(cache_value_string);
        table->table[index]->value = (cache_value_string*)malloc(size);
        memcpy((cache_value_string*)table->table[index]->value, (cache_value_string*)value, size);
    }

    return true;
}

cache_value_t* cache_table_get(cache_table_t* table, cache_value_key key)
{
    int64_t index = fnv1a(key) % table->size;

    return table->table[index];
}

cache_value_t* cache_table_inc(cache_table_t* table, cache_value_key key)
{
    cache_value_t* value = cache_table_get(table, key);

    if (value == NULL) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_STRING) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_DECIMAL) {
        cache_value_decimal* val = (cache_value_decimal*)value->value;
        *val = *val + 1;
    } else if (value->type == CACHE_VALUE_REAL) {
        cache_value_real* val = (cache_value_real*)value->value;
        *val = *val + 1;
    }

    return value;
}

cache_value_t* cache_table_dec(cache_table_t* table, cache_value_key key)
{
    cache_value_t* value = cache_table_get(table, key);

    if (value == NULL) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_STRING) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_DECIMAL) {
        cache_value_decimal* val = (cache_value_decimal*)value->value;
        *val = *val - 1;
    } else if (value->type == CACHE_VALUE_REAL) {
        cache_value_real* val = (cache_value_real*)value->value;
        *val = *val - 1;
    }

    return value;
}

bool cache_table_del(cache_table_t* table, cache_value_key key)
{
    int64_t index = fnv1a(key) % table->size;
    cache_value_t* value = table->table[index];

    if (value == NULL) {
        return false;
    }

    free(value->value);
    free(value);
    table->table[index] = NULL;

    return true;
}
