#ifndef LIBMEMUNCACHED_H
#define LIBMEMUNCACHED_H

#include "logger.h"
#include "utils.h"

#include <arpa/inet.h>
#include <fcntl.h>
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

/**
 * @brief increment value
 *
 * @param client
 * @param key
 * @param [offset]
 * @param [initial]
 */
void __memuncached_inc(memuncached_client_t* client, char* key, ...);
#define memuncached_inc(client, key, ...) __memuncached_inc(client, key, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)

/**
 * @brief decrement value
 *
 * @param client
 * @param key
 * @param [offset]
 * @param [initial]
 */
void __memuncached_dec(memuncached_client_t* client, char* key, ...);
#define memuncached_dec(client, key, ...) __memuncached_dec(client, key, ##__VA_ARGS__, LIBMEMUNCACHED_DEFAULT_INT, LIBMEMUNCACHED_DEFAULT_INT)

/**
 * @brief deletes value identified by key
 *
 * @param client
 * @param key
 */
void memuncached_del(memuncached_client_t* client, char* key);

/**
 * @brief disconnect
 *
 * @param client
 * @return true
 * @return false
 */
bool memuncached_disconnect(memuncached_client_t* client);

#endif