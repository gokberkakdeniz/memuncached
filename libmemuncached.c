#include "libmemuncached.h"

bool memuncached_recv(memuncached_client_t* client)
{
    client->response.body = NULL;
    client->response.body_size = 0;
    client->response.code = 0;
    client->response.description = NULL;

    LOG_DEBUG("memuncached_recv start");

    int read_size = 0;
    int total_size = 0;
    int payload_read_size = 0;

    int line_number = 0;
    char* line_start = client->buffer;

    while (true) {
        LOG_DEBUG("memuncached_recv loop");

        read_size = recv(client->fd, client->buffer + total_size, client->buffer_size - total_size, 0);

        if (read_size == 0) {
            LOG_FATAL("client disconnected.");
            return false;
        }

        if (read_size == -1) {
            LOG_FATAL("recv failed.");
            return false;
        }

        total_size += read_size;

        if (line_number == 0) {
            LOG_DEBUG("line_number %d", line_number);

            char* itr_end = client->buffer + total_size;
            for (char* itr = line_start; itr < itr_end; itr++) {
                if (*itr == '\n') {
                    line_number++;

                    // parse response code
                    line_start[3] = 0;
                    client->response.code = atoi(line_start);
                    // parse response description
                    line_start += 4;
                    *(itr - 1) = 0;
                    client->response.description = line_start;
                    // update line start
                    line_start = itr + 1;

                    // done
                    LOG_DEBUG("response.code %d", client->response.code);
                    LOG_DEBUG("response.description %s", client->response.description);
                    break;
                }
            }
        }

        if (line_number == 1) {
            LOG_DEBUG("line_number %d", line_number);

            char* itr_end = client->buffer + total_size;
            for (char* itr = line_start; itr < itr_end; itr++) {
                if (*itr == '\n') {
                    line_number++;

                    // parse body_size
                    *(itr - 1) = 0;
                    client->response.body_size = atoi(line_start);

                    // update line start
                    line_start = itr + 1;
                    client->response.body = line_start;

                    // done
                    LOG_DEBUG("response.code %d", client->response.code);
                    LOG_DEBUG("response.description %s", client->response.description);
                    LOG_DEBUG("response.body_size %d", client->response.body_size);
                    break;
                }
            }
        }

        if (line_number == 2) {
            LOG_DEBUG("line_number %d", line_number);

            payload_read_size += read_size;

            if (payload_read_size >= client->response.body_size + 3) {
                LOG_DEBUG("response.code %d", client->response.code);
                LOG_DEBUG("response.description %s", client->response.description);
                LOG_DEBUG("response.body_size %d", client->response.body_size);
                LOG_DEBUG("payload_read_size %d", payload_read_size);

                printf("===========BODY============\n");
                for (size_t i = 0; i < client->response.body_size; i++) {
                    printf("%c", client->response.body[i]);
                }
                printf("\n===========BODY============\n");
                break;
            }
        }

        int free_len = client->buffer_size - (total_size + 1);
        if (free_len < 0) {
            client->buffer_size *= 2;
            char* buffer_new = (char*)realloc(client->buffer, client->buffer_size);

            if (buffer_new == NULL) {
                LOG_FATAL("Realloc failed.");
                return false;
            }

            client->response.description = buffer_new + (client->response.description - client->buffer);
            client->response.body = buffer_new + (client->response.body - client->buffer);
            client->buffer = buffer_new;
        }

        LOG_DEBUG("total_size %d", total_size);
    }

    printf("===========BUFFER============\n");
    for (size_t i = 0; i < total_size; i++) {
        printf("%c", client->buffer[i]);
    }
    printf("\n===========BUFFER============\n");

    LOG_DEBUG("memuncached_recv end");

    return true;
}

memuncached_client_t* memuncached_connect(const char* ip, int port, const char* username, const char* password)
{
    memuncached_client_t* client = (memuncached_client_t*)malloc(sizeof(memuncached_client_t));

    if (client == NULL) {
        goto clean_and_return;
    }

    client->response.body = NULL;
    client->response.body_size = 0;
    client->response.code = 0;
    client->response.description = NULL;
    client->buffer_size = 10;
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

    char buffer[100];
    int size = recv(client->fd, buffer, 100, 0);
    if (size < 2) {
        goto clean_and_return;
    }
    buffer[size - 2] = 0;
    LOG_INFO("Connected to %s:%d (%s)", ip, port, buffer);

    return client;

clean_and_return:
    if (client != NULL)
        free(client);

    return NULL;
}

bool memuncached_stt(memuncached_client_t* client, memuncached_stt_result_t* result)
{
    dprintf(client->fd, "STT\r\n");

    bool is_success = memuncached_recv(client);

    if (is_success) {
        // I know it is inefficient :/
        result->client_count = atoi(sstrstr(client->response.body, "Client-Count: ", client->response.body_size) + 14);
        result->key_count = atoi(sstrstr(client->response.body, "Key-Count: ", client->response.body_size) + 11);
    }

    return is_success;
}

bool memuncached_ver(memuncached_client_t* client, memuncached_ver_result_t* result)
{
    dprintf(client->fd, "VER\r\n");

    bool is_success = memuncached_recv(client);

    if (is_success) {
        client->response.body[client->response.body_size] = 0;
        strcpy(result->version, client->response.body);
    }

    return is_success;
}

void __memuncached_inc(memuncached_client_t* client, char* key, ...)
{
    va_list opt_args;
    va_start(opt_args, key);

    int offset = va_arg(opt_args, int);
    int initial = va_arg(opt_args, int);

    if (offset == LIBMEMUNCACHED_DEFAULT_INT) {
        LOG_DEBUG("%d\n", dprintf(client->fd, "INC %s\r\n", key));
    } else if (initial == LIBMEMUNCACHED_DEFAULT_INT) {
        LOG_DEBUG("%d\n", dprintf(client->fd, "INC %s %d\r\n", key, offset));
    } else {
        LOG_DEBUG("%d\n", dprintf(client->fd, "INC %s %d %d\r\n", key, offset, initial));
    }

    va_end(opt_args);

    int size = recv(client->fd, client->buffer, client->buffer_size, 0);
    if (size < 2) {
        size = 0;
    }
    client->buffer[size - 2] = 0;
    LOG_INFO("msg: %s", client->buffer);
}

void __memuncached_dec(memuncached_client_t* client, char* key, ...)
{
    va_list opt_args;
    va_start(opt_args, key);

    int offset = va_arg(opt_args, int);
    int initial = va_arg(opt_args, int);

    if (offset == LIBMEMUNCACHED_DEFAULT_INT) {
        LOG_DEBUG("%d\n", dprintf(client->fd, "DEC %s\r\n", key));
    } else if (initial == LIBMEMUNCACHED_DEFAULT_INT) {
        LOG_DEBUG("%d\n", dprintf(client->fd, "DEC %s %d\r\n", key, offset));
    } else {
        LOG_DEBUG("%d\n", dprintf(client->fd, "DEC %s %d %d\r\n", key, offset, initial));
    }

    va_end(opt_args);

    int size = recv(client->fd, client->buffer, client->buffer_size, 0);
    if (size < 2) {
        size = 0;
    }
    client->buffer[size - 2] = 0;
    LOG_INFO("msg: %s", client->buffer);
}

void memuncached_del(memuncached_client_t* client, char* key)
{
    LOG_DEBUG("%d\n", dprintf(client->fd, "DEL %s\r\n", key));
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
