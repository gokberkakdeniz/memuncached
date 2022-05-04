#define LOG_LEVEL LOG_LEVEL_TRACE

#include "auth.h"
#include "connection.h"
#include "defs.h"
#include "hash_table.h"
#include "logger.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void* handle_connection(void* socket_desc);
void handle_sigint(int _);

volatile bool is_running = true;
hash_table_t* table;

int main(int argc, const char** argv)
{
    LOG_INFO(MEMUNCACHED_HEADER);
    LOG_INFO("server starting...");

    signal(SIGINT, handle_sigint);

    table = hash_table_create(MEMUNCACHED_TABLE_INITIAL_SIZE);
    LOG_INFO("cache table with size %d created.", MEMUNCACHED_TABLE_INITIAL_SIZE);

    int port = 9999;
    struct sockaddr_in socket_addr = {
        .sin_family = AF_INET,
        .sin_addr = INADDR_ANY,
        .sin_port = htons(port)
    };
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        LOG_FATAL("could not create socket. (errno=%d, err=%s) closing...", errno, strerror(errno));
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int) { 1 }, sizeof(int)) < 0) {
        LOG_FATAL("could not set option of the socket. (errno=%d, err=%s) closing...", errno, strerror(errno));
        exit(1);
    }

    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1) {
        LOG_FATAL("could not be bind (errno=%d, err=%s). closing...", errno, strerror(errno));
        exit(1);
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        LOG_FATAL("could not listen (errno=%d, err=%s). closing...", errno, strerror(errno));
        exit(1);
    }

    LOG_INFO("listening on port %d.", port);

    struct sockaddr_in socket_client_addr;
    size_t socket_client_addr_len = sizeof(socket_client_addr);
    char client_addr[INET_ADDRSTRLEN];
    int client_port, socket_client_fd;
    pthread_t thread_id;
    int thread_err;

    while (is_running) {
        socket_client_fd = accept(socket_fd, (struct sockaddr*)&socket_client_addr, (socklen_t*)&socket_client_addr_len);
        inet_ntop(AF_INET, &socket_client_addr.sin_addr, client_addr, sizeof(client_addr));
        client_port = ntohs(socket_client_addr.sin_port);

        if (socket_client_fd == -1) {
            if (errno == EINTR) {
                break;
            }

            LOG_ERROR("%s:%d - connot accept connection. (errno=%d, err=%s)", client_addr, client_port, errno, strerror(errno));
            continue;
        }

        client_connection_t* args = malloc(sizeof(client_connection_t));

        args->socket_fd = socket_client_fd;
        args->port = client_port;

        strcpy(args->addr, client_addr);

        if ((thread_err = pthread_create(&thread_id, NULL, handle_connection, (void*)args)) != 0) {
            args->thread_id = thread_id;
            LOG_ERROR("%s:%d - thread could not spawned. (errno=%d, err=%s)", client_addr, client_port, thread_err, strerror(thread_err));
            close(socket_client_fd);
        } else {
            LOG_INFO("%s:%d - connection accepted.", client_addr, client_port);
        }
    }

    close(socket_fd);
    LOG_INFO("socket closed.");

    hash_table_destroy(table);
    LOG_INFO("table destroyed.");

    return 0;
}

void handle_sigint(int _)
{
    is_running = false;
}