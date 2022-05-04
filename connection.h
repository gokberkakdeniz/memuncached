#ifndef H_CONNECTION
#define H_CONNECTION

/**
 * @file connection.h
 * @author Gokberk Akdeniz
 * @brief Parse messages, handle client connections
 * @version 0.1
 * @date 2022-05-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "defs.h"
#include "logger.h"
#include "utils.h"

#include <arpa/inet.h>
#include <errno.h>
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

extern hash_table_t* table;

#define CONNECTION_RECV_LENGTH 10

#define LOG_CLIENT_FORMAT "%s:%d (#%lu) - "
#define LOG_CLIENT_FORMAT_ARGS client->addr, client->port, client->thread_id
#define LOG_CLIENT_ERROR_FORMAT "(errno=%d, err=%s)"
#define LOG_CLIENT_ERROR_FORMAT_ARGS errno, strerror(errno)

#define RESPONSE_400_BAD_REQUEST "400 BAD REQUEST"
#define RESPONSE_200_OK "200 OK"

#define __RESPONSE_DATA(code, data) code "\r\n" data "\r\n\0"
#define RESPONSE_WRITE(sock, code, data) write(sock, __RESPONSE_DATA(code, data), strlen(__RESPONSE_DATA(code, data)))

#define REPLY_BAD_REQUEST(sock, command, message, received)                                                                                 \
    {                                                                                                                                       \
        LOG_INFO(LOG_CLIENT_FORMAT "bad request. (cmd='%s', msg='%s', received='%s')", LOG_CLIENT_FORMAT_ARGS, command, message, received); \
        RESPONSE_WRITE(sock, RESPONSE_400_BAD_REQUEST, message);                                                                            \
    }

#define REPLY_SUCCESS(sock, data)                                                                        \
    {                                                                                                    \
        LOG_INFO(LOG_CLIENT_FORMAT "status=%s, data=%s", LOG_CLIENT_FORMAT_ARGS, RESPONSE_200_OK, data); \
        RESPONSE_WRITE(sock, RESPONSE_200_OK, data);                                                     \
    }

/**
 * @brief client connection handler thread argument
 */
typedef struct client_connection {
    int socket_fd;
    char addr[INET_ADDRSTRLEN];
    int port;
    pthread_t thread_id;
} client_connection_t;

void* handle_connection(void* arg);

void handle_command(const char* command, client_connection_t* client);

void memuncached_bye(client_connection_t* client);

void memuncached_stt(client_connection_t* client);

void memuncached_ver(client_connection_t* client);

#endif