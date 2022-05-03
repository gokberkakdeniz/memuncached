#include "auth.h"
#include "cache_table.h"
#include "command.h"
#include <stdbool.h>
#include <stdio.h>

#define LOG_LEVEL LOG_LEVEL_TRACE
#include "logger.h"

int main(int argc, const char** argv)
{
    printf("%s\n", crypt("123456", "$1$salt"));

    LOG_FATAL("log=%d\n", (int)LOG_LEVEL);
    LOG_ERROR("log=%d\n", (int)LOG_LEVEL);
    LOG_WARN("log=%d\n", (int)LOG_LEVEL);
    LOG_INFO("log=%d\n", (int)LOG_LEVEL);
    LOG_DEBUG("log=%d\n", (int)LOG_LEVEL);
    LOG_TRACE("log=%d\n", (int)LOG_LEVEL);

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

    command_parse("SET xyz 123");
    command_parse("STAT");
    command_parse("STAT S");

    return 0;
}