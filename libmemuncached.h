#ifndef LIBMEMUNCACHED_H
#define LIBMEMUNCACHED_H

#include "logger.h"
#include "utils.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define LIBMEMUNCACHED_DEFAULT_INT INT_MIN
#define LIBMEMUNCACHED_DEFAULT_DOUBLE DBL_MIN
#define LIBMEMUNCACHED_RECV_LENGTH 1000

typedef struct memuncached_client {
    int fd;
    struct sockaddr_in addr;
    char* buffer;
    int buffer_size;
    struct {
        int code;
        char* description;
        size_t body_size;
        char* body;
        char body_type;
    } response;
} memuncached_client_t;

memuncached_client_t* memuncached_connect(const char* ip, int port, const char* username, const char* password);

typedef struct memuncached_stt_result {
    int client_count;
    int key_count;
} memuncached_stt_result_t;

/**
 * @brief get statistics
 *
 * @param client
 */
bool memuncached_stt(memuncached_client_t* client, memuncached_stt_result_t* result);

typedef struct memuncached_ver_result {
    char version[5];
} memuncached_ver_result_t;

/**
 * @brief get server version
 *
 * @param client
 */
bool memuncached_ver(memuncached_client_t* client, memuncached_ver_result_t* result);

#define MEMCACHED_NUMBER_RESULT_DECIMAL 0
#define MEMCACHED_NUMBER_RESULT_REAL 1
#define MEMCACHED_NUMBER_RESULT_STRING 2

typedef struct memuncached_value_result {
    char type;
    union {
        double real;
        int64_t decimal;
        char* string;
    };
    int size;
} memuncached_value_result_t;

/**
 * @brief frees value if type is string;
 *
 * @param result
 * @return bool
 */
bool memuncached_value_result_clean(memuncached_value_result_t* result);

/**
 * @brief increment value
 *
 * @param client
 * @param key
 * @param type
 * @param [offset]
 * @param [initial]
 */
bool __memuncached_inc(memuncached_client_t* client, char* key, char type, memuncached_value_result_t* result, ...);
#define memuncached_inc_d(client, key, result, ...) __memuncached_inc(client, key, MEMCACHED_NUMBER_RESULT_DECIMAL, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)
#define memuncached_inc_f(client, key, result, ...) __memuncached_inc(client, key, MEMCACHED_NUMBER_RESULT_REAL, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_DOUBLE, LIBMEMUNCACHED_DEFAULT_DOUBLE)

/**
 * @brief decrement value
 *
 * @param client
 * @param key
 * @param [offset]
 * @param [initial]
 */
bool __memuncached_dec(memuncached_client_t* client, char* key, char type, memuncached_value_result_t* result, ...);
#define memuncached_dec_d(client, key, result, ...) __memuncached_dec(client, key, MEMCACHED_NUMBER_RESULT_DECIMAL, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)
#define memuncached_dec_f(client, key, result, ...) __memuncached_dec(client, key, MEMCACHED_NUMBER_RESULT_REAL, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_DOUBLE, LIBMEMUNCACHED_DEFAULT_DOUBLE)

/**
 * @brief deletes value identified by key
 *
 * @param client
 * @param key
 * @param result
 * @return bool
 */
bool memuncached_del(memuncached_client_t* client, char* key, memuncached_value_result_t* result);

/**
 * @brief gets value identified by key
 *
 * @param client
 * @param key
 * @param result
 * @return bool
 */
bool memuncached_get(memuncached_client_t* client, char* key, memuncached_value_result_t* result);

/**
 * @brief sets value identified by key
 *
 * @param client
 * @param key
 * @param data
 * @param [length]
 * @return bool
 */
bool __memuncached_set(memuncached_client_t* client, char* key, char type, void* data, int length);
#define memuncached_set_d(client, key, data) __memuncached_set(client, key, MEMCACHED_NUMBER_RESULT_DECIMAL, data, snprintf(NULL, 0, "%ld", *((int64_t*)data)))
#define memuncached_set_f(client, key, data) __memuncached_set(client, key, MEMCACHED_NUMBER_RESULT_REAL, data, snprintf(NULL, 0, "%lf", *((double*)data)))
#define memuncached_set_s(client, key, data, length) __memuncached_set(client, key, MEMCACHED_NUMBER_RESULT_STRING, data, length)

/**
 * @brief sets value identified by key, if exist return false
 *
 * @param client
 * @param key
 * @param data
 * @param [length]
 * @return bool
 */
bool __memuncached_add(memuncached_client_t* client, char* key, char type, void* data, int length);
#define memuncached_add_d(client, key, data) __memuncached_add(client, key, MEMCACHED_NUMBER_RESULT_DECIMAL, data, snprintf(NULL, 0, "%ld", *((int64_t*)data)))
#define memuncached_add_f(client, key, data) __memuncached_add(client, key, MEMCACHED_NUMBER_RESULT_REAL, data, snprintf(NULL, 0, "%lf", *((double*)data)))
#define memuncached_add_s(client, key, data, length) __memuncached_add(client, key, MEMCACHED_NUMBER_RESULT_STRING, data, length)

/**
 * @brief disconnect
 *
 * @param client
 * @return true
 * @return false
 */
bool memuncached_disconnect(memuncached_client_t* client);

#endif