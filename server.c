#define LOG_LEVEL LOG_LEVEL_TRACE

#include "auth.h"
#include "connection.h"
#include "defs.h"
#include "hash_table.h"
#include "logger.h"
#include "vector.h"

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
vector_t* clients;

int main(int argc, const char** argv)
{
    LOG_INFO(MEMUNCACHED_HEADER);
    LOG_INFO("server starting...");

    signal(SIGINT, handle_sigint);

    table = hash_table_create(MEMUNCACHED_TABLE_INITIAL_SIZE);
    LOG_INFO("cache table with size %d created.", MEMUNCACHED_TABLE_INITIAL_SIZE);

    clients = vector_create(10);

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

        client_connection_t* client = malloc(sizeof(client_connection_t));

        client->socket_fd = socket_client_fd;
        client->port = client_port;
        client->is_thread_running = true;

        strcpy(client->addr, client_addr);

        if ((thread_err = pthread_create(&thread_id, NULL, handle_connection, (void*)client)) != 0) {
            LOG_ERROR("%s:%d - thread could not spawned. (errno=%d, err=%s)", client_addr, client_port, thread_err, strerror(thread_err));
            close(socket_client_fd);
            free(client);
        } else {
            if (!vector_insert(clients, (void*)client)) {
                LOG_ERROR("%s:%d - could not insert client to clients vector. (errno=%d, err=%s)", client_addr, client_port, errno, strerror(errno));
                LOG_DEBUG("%s:%d - clients.size = %lu, clients.count = %lu.", client_addr, client_port, clients->size, clients->count);
            }
            LOG_INFO("%s:%d - connection accepted.", client_addr, client_port);
        }
    }

    LOG_INFO("closing...");

    vector_lock(clients);
    client_connection_t* client;
    for (size_t i = 0; i < clients->count; i++) {
        client = (client_connection_t*)vector_at_unsafe(clients, i);
        pthread_join(client->thread_id, NULL);
    }
    vector_unlock(clients);

    close(socket_fd);
    LOG_DEBUG("socket closed.");

    hash_table_destroy(table);
    LOG_DEBUG("table destroyed.");

    vector_destroy(clients);
    LOG_DEBUG("clients vector destroyed.");

    return 0;
}

void handle_sigint(int _)
{
    is_running = false;
}