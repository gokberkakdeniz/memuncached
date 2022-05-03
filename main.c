#include "cache_table.h"
#include <stdbool.h>
#include <stdio.h>

int main(int argc, const char** argv)
{
    cache_table_t* t = cache_table_create(10);

    int abc_val = 99;
    cache_table_set(t, "abc", 0, &abc_val);
    cache_value_t* v = cache_table_get(t, "abc");
    printf("%s = %ld\n", v->key, *(cache_value_decimal*)v->value);

    char* def_val = "deneme bir iki uc.";
    cache_table_set(t, "def", 2, &def_val);
    v = cache_table_get(t, "def");
    printf("%s = %s\n", v->key, *(cache_value_string*)v->value);

    cache_table_inc(t, "abc");
    v = cache_table_get(t, "abc");
    printf("%s = %ld\n", v->key, *(cache_value_decimal*)v->value);

    printf("del? = %d\n", cache_table_del(t, "abc"));

    cache_table_destroy(t);

    return 0;
}