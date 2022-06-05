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
#define memuncached_inc_d(client, key, result, ...) __memuncached_inc(client, key, 0, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)
#define memuncached_inc_f(client, key, result, ...) __memuncached_inc(client, key, 1, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_DOUBLE, LIBMEMUNCACHED_DEFAULT_DOUBLE)

/**
 * @brief decrement value
 *
 * @param client
 * @param key
 * @param [offset]
 * @param [initial]
 */
bool __memuncached_dec(memuncached_client_t* client, char* key, char type, memuncached_value_result_t* result, ...);
#define memuncached_dec_d(client, key, result, ...) __memuncached_dec(client, key, 0, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)
#define memuncached_dec_f(client, key, result, ...) __memuncached_dec(client, key, 1, result, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_DOUBLE, LIBMEMUNCACHED_DEFAULT_DOUBLE)

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
 * @brief disconnect
 *
 * @param client
 * @return true
 * @return false
 */
bool memuncached_disconnect(memuncached_client_t* client);

#endif