#include "libmemuncached.h"

memuncached_client_t* memuncached_connect(const char* ip, int port, const char* username, const char* password)
{
    memuncached_client_t* client = (memuncached_client_t*)malloc(sizeof(memuncached_client_t));

    if (client == NULL) {
        goto clean_and_return;
    }

    client->buffer_size = 1024;
    client->buffer = (char*)calloc(client->buffer_size, sizeof(char));

    if (client->buffer == NULL) {
        goto clean_and_return;
    }

    if ((client->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        goto clean_and_return;
    }

    client->addr.sin_family = AF_INET;
    client->addr.sin_port = htons(port);

    if ((client->addr.sin_addr.s_addr = inet_addr(ip)) == -1) {
        goto clean_and_return;
    }

    if (connect(client->fd, (struct sockaddr*)&client->addr, sizeof(client->addr)) == -1) {
        goto clean_and_return;
    }

    int size = recv(client->fd, client->buffer, client->buffer_size, 0);
    if (size < 2) {
        goto clean_and_return;
    }
    client->buffer[size - 2] = 0;
    LOG_INFO("Connected to %s:%d (%s)", ip, port, client->buffer);

    return client;

clean_and_return:
    if (client != NULL)
        free(client);

    return NULL;
}

void memuncached_stt(memuncached_client_t* client)
{
    LOG_DEBUG("%d\n", dprintf(client->fd, "stt\r\n"));
    int size = recv(client->fd, client->buffer, client->buffer_size, 0);
    if (size < 2) {
        size = 0;
    }
    client->buffer[size - 2] = 0;
    LOG_INFO("msg: %s", client->buffer);
}

bool memuncached_disconnect(memuncached_client_t* client)
{
    LOG_DEBUG("%d\n", dprintf(client->fd, "BYE\r\n"));

    int size = recv(client->fd, client->buffer, client->buffer_size, 0);
    if (size < 2) {
        size = 0;
    }
    client->buffer[size - 2] = 0;
    LOG_INFO("msg: %s", client->buffer);

    int result = shutdown(client->fd, SHUT_WR) + shutdown(client->fd, SHUT_RD) + close(client->fd);
    free(client->buffer);
    free(client);
    return result == 0;
}
