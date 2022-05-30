#include "hash_table.h"

hash_table_t* hash_table_create(size_t size)
{
    hash_table_t* table_ptr = (hash_table_t*)malloc(sizeof(hash_table_t));

    table_ptr->table = (cache_value_t**)calloc(size, sizeof(cache_value_t*));
    table_ptr->size = size;
    table_ptr->count = 0;

    return table_ptr;
}

void hash_table_destroy(hash_table_t* table)
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

bool hash_table_set(hash_table_t* table, cache_value_key key, cache_value_type type, void* value, size_t size)
{
    int64_t index = fnv1a(key) % table->size;

    if (table->table[index] == NULL) {
        table->table[index] = (cache_value_t*)malloc(sizeof(cache_value_t));
        table->table[index]->value = NULL;
    }

    table->table[index]->key = key;
    table->table[index]->type = type;
    table->table[index]->length = size;

    if (table->table[index]->value != NULL) {
        free(table->table[index]->value);
    }

    if (type == CACHE_VALUE_DECIMAL) {
        table->table[index]->value = (cache_value_decimal*)malloc(sizeof(cache_value_decimal));
        *(cache_value_decimal*)table->table[index]->value = *(cache_value_decimal*)value;
    } else if (type == CACHE_VALUE_REAL) {
        table->table[index]->value = (cache_value_real*)malloc(sizeof(cache_value_real));
        *(cache_value_real*)table->table[index]->value = *(cache_value_real*)value;
    } else if (type == CACHE_VALUE_STRING) {
        table->table[index]->value = (cache_value_string*)calloc(size, sizeof(char));
        memcpy((cache_value_string*)table->table[index]->value, (cache_value_string*)value, size);
    }

    return true;
}

cache_value_t* hash_table_get(hash_table_t* table, cache_value_key key)
{
    int64_t index = fnv1a(key) % table->size;

    return table->table[index];
}

cache_value_t* hash_table_inc(hash_table_t* table, cache_value_key key, cache_value_real offset)
{
    cache_value_t* value = hash_table_get(table, key);

    if (value == NULL) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_STRING) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_DECIMAL) {
        cache_value_decimal* val = (cache_value_decimal*)value->value;
        *val = *val + offset;
    } else if (value->type == CACHE_VALUE_REAL) {
        cache_value_real* val = (cache_value_real*)value->value;
        *val = *val + offset;
    }

    return value;
}

cache_value_t* hash_table_dec(hash_table_t* table, cache_value_key key, cache_value_real offset)
{
    cache_value_t* value = hash_table_get(table, key);

    if (value == NULL) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_STRING) {
        return NULL;
    }

    if (value->type == CACHE_VALUE_DECIMAL) {
        cache_value_decimal* val = (cache_value_decimal*)value->value;
        *val = *val - offset;
    } else if (value->type == CACHE_VALUE_REAL) {
        cache_value_real* val = (cache_value_real*)value->value;
        *val = *val - offset;
    }

    return value;
}

bool hash_table_del(hash_table_t* table, cache_value_key key)
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
