#define LOG_LEVEL LOG_LEVEL_TRACE

#include "auth.h"
#include "cache_table.h"
#include "command.h"
#include "defs.h"
#include "logger.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void* handle_connection(void* socket_desc);

typedef struct client_connection {
    int socket_fd;
    char addr[INET_ADDRSTRLEN];
    int port;
} client_connection_t;

int main(int argc, const char** argv)
{
    LOG_INFO(MEMUNCACHED_HEADER);
    LOG_INFO("server starting...");

    int port = 9999;
    struct sockaddr_in socket_addr = {
        .sin_family = AF_INET,
        .sin_addr = INADDR_ANY,
        .sin_port = htons(port)
    };
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        LOG_FATAL("could not create socket. errno=%d, err=%s. closing...", errno, strerror(errno));
        exit(1);
    }

    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1) {
        LOG_FATAL("could not be bind. errno=%d, err=%s. closing...", errno, strerror(errno));
        exit(1);
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        LOG_FATAL("could not listen. errno=%d, err=%s. closing...", errno, strerror(errno));
        exit(1);
    }

    LOG_INFO("listening on port %d.", port);

    struct sockaddr_in socket_client_addr;
    size_t socket_client_addr_len = sizeof(socket_client_addr);
    char client_addr[INET_ADDRSTRLEN];
    int client_port, socket_client_fd;
    pthread_t thread_id;
    while (true) {
        socket_client_fd = accept(socket_fd, (struct sockaddr*)&socket_client_addr, (socklen_t*)&socket_client_addr_len);
        inet_ntop(AF_INET, &socket_client_addr.sin_addr, client_addr, sizeof(client_addr));
        client_port = ntohs(socket_client_addr.sin_port);

        if (socket_client_fd == -1) {
            LOG_ERROR("%s:%d - connot accept connection.", client_addr, client_port);
            exit(1);
        }

        client_connection_t args = {
            .socket_fd = socket_client_fd,
            .port = client_port
        };

        strcpy(args.addr, client_addr);

        if (pthread_create(&thread_id, NULL, handle_connection, (void*)&args) != 0) {
            LOG_ERROR("%s:%d - thread could not spawned.", client_addr, client_port);
        } else {
            LOG_INFO("%s:%d - connection accepted.", client_addr, client_port);
        }
    }

    return 0;
}

void* handle_connection(void* vconn)
{
    //Get the socket descriptor
    client_connection_t* conn = (client_connection_t*)vconn;

    int read_size;
    char *message, client_message[2000];

    //Send some messages to the client
    message = MEMUNCACHED_HEADER "\n";
    write(conn->socket_fd, message, strlen(message));

    //Receive a message from client
    while ((read_size = recv(conn->socket_fd, client_message, 2000, 0)) > 0) {
        //end of string marker
        client_message[read_size] = '\0';

        //Send the message back to client
        write(conn->socket_fd, client_message, strlen(client_message));

        //clear the message buffer
        memset(client_message, 0, 2000);
    }

    if (read_size == 0) {
        LOG_INFO("%s:%d - client disconnected.", conn->addr, conn->port);
        fflush(stdout);
    } else if (read_size == -1) {
        LOG_ERROR("%s:%d - recv failed. errno=%d, err=%s", conn->addr, conn->port, errno, strerror(errno));
    }

    return 0;
}