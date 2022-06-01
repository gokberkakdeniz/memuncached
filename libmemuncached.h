#ifndef LIBMEMUNCACHED_H
#define LIBMEMUNCACHED_H

#include "logger.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define LIBMEMUNCACHED_DEFAULT_INT INT_MIN

typedef struct memuncached_client {
    int fd;
    struct sockaddr_in addr;
    char* buffer;
    int buffer_size;
} memuncached_client_t;

memuncached_client_t* memuncached_connect(const char* ip, int port, const char* username, const char* password);

/**
 * @brief get statistics
 *
 * @param client
 */
void memuncached_stt(memuncached_client_t* client);

/**
 * @brief get server version
 *
 * @param client
 */
void memuncached_ver(memuncached_client_t* client);

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