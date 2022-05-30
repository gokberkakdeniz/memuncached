#ifndef H_CONNECTION
#define H_CONNECTION

/**
 * @file connection.h
 * @author Gokberk Akdeniz
 * @brief Parse messages, handle client connections, protocol declarations
 * @version 0.1
 * @date 2022-05-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "defs.h"
#include "logger.h"
#include "utils.h"
#include "vector.h"

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

static const char CLIENT_WELCOME_MESSAGE[] = MEMUNCACHED_HEADER "\n";
static const size_t CLIENT_WELCOME_MESSAGE_LEN = sizeof(CLIENT_WELCOME_MESSAGE) / sizeof(CLIENT_WELCOME_MESSAGE[0]);

#define CONNECTION_RECV_LENGTH 10

#define LOG_CLIENT_FORMAT "%s:%d (#%lu) - "
#define LOG_CLIENT_FORMAT_ARGS client->addr, client->port, client->thread_id
#define LOG_CLIENT_ERROR_FORMAT "(errno=%d, err=%s)"
#define LOG_CLIENT_ERROR_FORMAT_ARGS errno, strerror(errno)

#define RESPONSE_200_OK "200 OK"
#define RESPONSE_400_BAD_REQUEST "400 BAD REQUEST"
#define RESPONSE_404_NOT_FOUND "404 NOT FOUND"
#define RESPONSE_409_KEY_EXISTS "409 KEY EXISTS"
#define RESPONSE_500_SERVER_ERROR "500 SERVER ERROR"

#define RESPONSE_WRITE(sock, code, data_format, ...) dprintf(sock, "%s\r\n%d\r\n" data_format "\r\n\0", code, snprintf(NULL, 0, data_format, ##__VA_ARGS__), ##__VA_ARGS__)

#define REPLY_BAD_REQUEST(sock, command, message, received)                                                                                 \
    {                                                                                                                                       \
        LOG_INFO(LOG_CLIENT_FORMAT "Bad Request. (cmd='%s', msg='%s', received='%s')", LOG_CLIENT_FORMAT_ARGS, command, message, received); \
        RESPONSE_WRITE(sock, RESPONSE_400_BAD_REQUEST, message);                                                                            \
    }

#define REPLY_SERVER_ERROR(sock, message_format, ...)                                                                     \
    {                                                                                                                     \
        LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_500_SERVER_ERROR ". " message_format, LOG_CLIENT_FORMAT_ARGS, ##__VA_ARGS__); \
        RESPONSE_WRITE(sock, RESPONSE_500_SERVER_ERROR, message_format, ##__VA_ARGS__);                                   \
    }

#define REPLY_NOT_FOUND(sock, key)                                                                   \
    {                                                                                                \
        LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_404_NOT_FOUND ". Key: %s", LOG_CLIENT_FORMAT_ARGS, key); \
        RESPONSE_WRITE(sock, RESPONSE_404_NOT_FOUND, "");                                            \
    }

#define REPLY_SUCCESS(sock, data_format, ...)                                                                               \
    {                                                                                                                       \
        LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_200_OK ". Data: >>>" data_format "<<<", LOG_CLIENT_FORMAT_ARGS, ##__VA_ARGS__); \
        RESPONSE_WRITE(sock, RESPONSE_200_OK, data_format, ##__VA_ARGS__);                                                  \
    }

#define REPLY_KEY_EXISTS(sock)                                                           \
    {                                                                                    \
        LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_409_KEY_EXISTS ".", LOG_CLIENT_FORMAT_ARGS); \
        RESPONSE_WRITE(sock, RESPONSE_409_KEY_EXISTS, "");                               \
    }

/**
 * @brief client connection handler thread argument
 */
typedef struct client_connection {
    int socket_fd;
    char addr[INET_ADDRSTRLEN];
    int port;
    pthread_t thread_id;
    bool is_thread_running;
} client_connection_t;

void* handle_connection(void* arg);

void handle_command(const char* command, client_connection_t* client);

void memuncached_bye(client_connection_t* client);

void memuncached_stt(client_connection_t* client);

void memuncached_ver(client_connection_t* client);

void memuncached_dec(client_connection_t* client, char* key, char* offset, char* initial);

void memuncached_inc(client_connection_t* client, char* key, char* offset, char* initial);

void memuncached_del(client_connection_t* client, char* key);

void memuncached_get(client_connection_t* client, char* key);

void memuncached_set(client_connection_t* client, char* key, char* type, char* length);

void memuncached_add(client_connection_t* client, char* key, char* type, char* length);

#endif