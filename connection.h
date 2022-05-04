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

#define CONNECTION_RECV_LENGTH 10

#define LOG_CLIENT_FORMAT "%s:%d (#%lu) - "
#define LOG_CLIENT_FORMAT_ARGS client->addr, client->port, client->thread_id
#define LOG_CLIENT_ERROR_FORMAT "(errno=%d, err=%s)"
#define LOG_CLIENT_ERROR_FORMAT_ARGS errno, strerror(errno)
#define INVALID_SYNTAX(command, message, received)                                                                                              \
    {                                                                                                                                           \
        LOG_ERROR(LOG_CLIENT_FORMAT "invalid syntax. (cmd='%s', msg='%s', received='%s')", LOG_CLIENT_FORMAT_ARGS, command, message, received); \
        write(client->socket_fd, "invalid\n", 9);                                                                                               \
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

void memuncached_stat(client_connection_t* client);

void memuncached_version(client_connection_t* client);

#endif