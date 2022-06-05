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

    struct sockaddr_in socket_client_addr = { 0 };
    size_t socket_client_addr_len = sizeof(socket_client_addr);
    int socket_client_fd;
    pthread_t thread_id;
    int thread_err;

    while (is_running) {
        client_connection_t* client = (client_connection_t*)malloc(sizeof(client_connection_t));
        client->socket_fd = accept(socket_fd, (struct sockaddr*)&socket_client_addr, (socklen_t*)&socket_client_addr_len);

        if (client->socket_fd == -1) {
            if (errno != EINTR) {
                LOG_ERROR("connot accept connection. (errno=%d, err=%s)", errno, strerror(errno));
            }

            free(client);

            continue;
        }

        inet_ntop(AF_INET, &socket_client_addr.sin_addr, client->addr, sizeof(client->addr));
        client->port = ntohs(socket_client_addr.sin_port);
        client->is_thread_running = true;

        if ((thread_err = pthread_create(&thread_id, NULL, handle_connection, (void*)client)) != 0) {
            LOG_ERROR("%s:%d - thread could not spawned. (errno=%d, err=%s)", client->addr, client->port, thread_err, strerror(thread_err));
            close(client->socket_fd);
            free(client);
        } else {
            if (!vector_insert(clients, (void*)client)) {
                LOG_ERROR("%s:%d - could not insert client to clients vector. (errno=%d, err=%s)", client->addr, client->port, errno, strerror(errno));
                LOG_DEBUG("%s:%d - clients.size = %lu, clients.count = %lu.", client->addr, client->port, clients->size, clients->count);
                // TODO: what should we do?
            }
            LOG_INFO("%s:%d - connection accepted.", client->addr, client->port);
        }
    }

    LOG_INFO("closing...");

    vector_lock(clients);
    if (clients->count > 0) {
        LOG_INFO("Waiting %lu %s to be closed...", clients->count, clients->count == 1 ? "thread" : "threads");
    }
    for (ssize_t i = clients->count - 1; i >= 0; i--) {
        client_connection_t* client = (client_connection_t*)vector_at_unsafe(clients, i);
        LOG_TRACE("%s:%d (#%lu)- waiting to be closed...", client->addr, client->port, client->thread_id);
        shutdown(client->socket_fd, SHUT_WR);
        shutdown(client->socket_fd, SHUT_RD);
        close(client->socket_fd);
        vector_remove_unsafe(clients, i);
        free(client);
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