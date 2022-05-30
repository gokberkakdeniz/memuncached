#ifndef LIBMEMUNCACHED_H
#define LIBMEMUNCACHED_H

#include "logger.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct memuncached_client {
    int fd;
    struct sockaddr_in addr;
    char* buffer;
    int buffer_size;
} memuncached_client_t;

memuncached_client_t* memuncached_connect(const char* ip, int port, const char* username, const char* password);

void memuncached_stt(memuncached_client_t* client);

bool memuncached_disconnect(memuncached_client_t* client);

#endif