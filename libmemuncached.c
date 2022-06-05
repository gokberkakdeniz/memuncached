#include "libmemuncached.h"

bool memuncached_recv(memuncached_client_t* client)
{
    client->response.body = NULL;
    client->response.body_size = 0;
    client->response.body_type = 0;
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

        LOG_DEBUG("((socket recv req size)): %d", client->buffer_size - total_size);

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
        LOG_DEBUG("total_size %d read_size %d", total_size, read_size);

        if (line_number == 0) {
            LOG_DEBUG("line_number %d", line_number);

            char* itr_end = client->buffer + total_size;
            for (char* itr = line_start; itr < itr_end; itr++) {
                if (*itr == '\n') {
                    line_number++;

                    // put string terminator
                    *(itr - 1) = 0;

                    // parse response code
                    line_start[3] = 0;
                    client->response.code = atoi(line_start);
                    // parse response description
                    line_start += 4;

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
            char* whitespace = false;
            char* itr_end = client->buffer + total_size;
            for (char* itr = line_start; itr < itr_end; itr++) {
                if (*itr == ' ') {
                    whitespace = itr;
                }

                if (*itr == '\n') {
                    line_number++;

                    // put string terminator
                    *(itr - 1) = 0;

                    // parse body_type
                    if (whitespace != NULL) {
                        *whitespace = 0;
                        client->response.body_type = atoi(whitespace + 1);
                    }

                    // parse body_size
                    client->response.body_size = atoi(line_start);

                    // update line start
                    line_start = itr + 1;
                    client->response.body = line_start;

                    // since we add this to payload_read_size we need to substract length of previous line
                    read_size = itr_end - line_start;

                    // done
                    LOG_DEBUG("response.code %d", client->response.code);
                    LOG_DEBUG("response.description %s", client->response.description);
                    LOG_DEBUG("response.body_size %d", client->response.body_size);
                    LOG_DEBUG("response.body_type %d", client->response.body_type);
                    break;
                }
            }
        }

        if (line_number == 2) {
            LOG_DEBUG("line_number %d", line_number);

            payload_read_size += read_size;

            LOG_DEBUG("payload_read_size %d", payload_read_size);

            // printf("===========X_BODY============\n");
            // for (size_t i = 0; i < payload_read_size; i++) {
            //     printf("%c", client->response.body[i]);
            // }
            // printf("\n===========X_BODY============\n");

            if (payload_read_size >= client->response.body_size + 3) {
                LOG_DEBUG("response.code %d", client->response.code);
                LOG_DEBUG("response.description %s", client->response.description);
                LOG_DEBUG("response.body_size %d", client->response.body_size);
                LOG_DEBUG("payload_read_size %d", payload_read_size);

                // printf("===========BODY============\n");
                // for (size_t i = 0; i < client->response.body_size; i++) {
                //     printf("%c", client->response.body[i]);
                // }
                // printf("\n===========BODY============\n");
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

        // printf("===========LOOPBUFFER============\n");
        // for (size_t i = 0; i < total_size; i++) {
        //     printf("%c", client->buffer[i]);
        // }
        // printf("\n===========LOOPBUFFER============\n");
    }

    // printf("===========BUFFER============\n");
    // for (size_t i = 0; i < total_size; i++) {
    //     printf("%d ", client->buffer[i]);
    // }
    // printf("\n");
    // for (size_t i = 0; i < total_size; i++) {
    //     printf("%c", client->buffer[i]);
    // }
    // printf("\n===========BUFFER============\n");

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
    is_success = is_success && client->response.code == 200;

    memset(result, 0, sizeof(memuncached_stt_result_t));

    if (is_success && result != NULL) {
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
    is_success = is_success && client->response.code == 200;

    memset(result, 0, sizeof(memuncached_ver_result_t));

    if (is_success && result != NULL) {
        client->response.body[client->response.body_size] = 0;
        strcpy(result->version, client->response.body);
    }

    return is_success;
}

bool __memuncached_inc(memuncached_client_t* client, char* key, char type, memuncached_value_result_t* result, ...)
{
    va_list opt_args;
    va_start(opt_args, result);

    if (type = 0) {
        int offset = va_arg(opt_args, int);
        int initial = va_arg(opt_args, int);

        if (offset == LIBMEMUNCACHED_DEFAULT_INT) {
            dprintf(client->fd, "INC %s\r\n", key);
        } else if (initial == LIBMEMUNCACHED_DEFAULT_INT) {
            dprintf(client->fd, "INC %s %d\r\n", key, offset);
        } else {
            dprintf(client->fd, "INC %s %d %d\r\n", key, offset, initial);
        }
    } else if (type = 1) {
        double offset = va_arg(opt_args, double);
        double initial = va_arg(opt_args, double);

        if (offset == LIBMEMUNCACHED_DEFAULT_DOUBLE) {
            dprintf(client->fd, "INC %s\r\n", key);
        } else if (initial == LIBMEMUNCACHED_DEFAULT_DOUBLE) {
            dprintf(client->fd, "INC %s %lf\r\n", key, offset);
        } else {
            dprintf(client->fd, "INC %s %lf %lf\r\n", key, offset, initial);
        }
    } else {
        va_end(opt_args);
        return false;
    }

    va_end(opt_args);

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    memset(result, 0, sizeof(memuncached_value_result_t));

    if (is_success && result != NULL) {
        client->response.body[client->response.body_size] = 0;
        result->type = client->response.body_type;
        if (client->response.body_type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            result->decimal = atoll(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_REAL) {
            result->real = atof(client->response.body);
        } else {
            LOG_FATAL("Expected numeric (0 or 1) but found %d.", result->type);
            return false;
        }
    }

    return is_success;
}

bool __memuncached_dec(memuncached_client_t* client, char* key, char type, memuncached_value_result_t* result, ...)
{
    va_list opt_args;
    va_start(opt_args, result);

    if (type == 0) {
        int offset = va_arg(opt_args, int);
        int initial = va_arg(opt_args, int);

        if (offset == LIBMEMUNCACHED_DEFAULT_INT) {
            dprintf(client->fd, "DEC %s\r\n", key);
        } else if (initial == LIBMEMUNCACHED_DEFAULT_INT) {
            dprintf(client->fd, "DEC %s %d\r\n", key, offset);
        } else {
            dprintf(client->fd, "DEC %s %d %d\r\n", key, offset, initial);
        }
    } else if (type == 1) {
        double offset = va_arg(opt_args, double);
        double initial = va_arg(opt_args, double);

        if (offset == LIBMEMUNCACHED_DEFAULT_DOUBLE) {
            dprintf(client->fd, "DEC %s\r\n", key);
        } else if (initial == LIBMEMUNCACHED_DEFAULT_DOUBLE) {
            dprintf(client->fd, "DEC %s %lf\r\n", key, offset);
        } else {
            dprintf(client->fd, "DEC %s %lf %lf\r\n", key, offset, initial);
        }
    } else {
        va_end(opt_args);
        return false;
    }

    va_end(opt_args);

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    memset(result, 0, sizeof(memuncached_value_result_t));

    if (is_success && result != NULL) {
        client->response.body[client->response.body_size] = 0;
        result->type = client->response.body_type;
        if (client->response.body_type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            result->decimal = atoll(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_REAL) {
            result->real = atof(client->response.body);
        } else {
            LOG_FATAL("Expected numeric (0 or 1) but found %d.", result->type);
            return false;
        }
    }

    return is_success;
}

bool memuncached_del(memuncached_client_t* client, char* key, memuncached_value_result_t* result)
{
    dprintf(client->fd, "DEL %s\r\n", key);

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    if (is_success && result != NULL) {
        result->type = client->response.body_type;
        if (client->response.body_type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            result->decimal = atoll(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_REAL) {
            result->real = atof(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_STRING) {
            result->string = (char*)calloc(client->response.body_size, sizeof(char));
            memcpy(result->string, client->response.body, client->response.body_size);
            result->size = client->response.body_size;
        } else {
            LOG_FATAL("Invalid type %d.", result->type);
            return false;
        }
    }

    return is_success;
}

bool memuncached_get(memuncached_client_t* client, char* key, memuncached_value_result_t* result)
{
    dprintf(client->fd, "GET %s\r\n", key);

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    if (is_success && result != NULL) {
        result->type = client->response.body_type;
        if (client->response.body_type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            result->decimal = atoll(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_REAL) {
            result->real = atof(client->response.body);
        } else if (client->response.body_type == MEMCACHED_NUMBER_RESULT_STRING) {
            result->string = (char*)calloc(client->response.body_size, sizeof(char));
            memcpy(result->string, client->response.body, client->response.body_size);
            result->size = client->response.body_size;
        } else {
            LOG_FATAL("Invalid type %d.", result->type);
            return false;
        }
    }

    return is_success;
}

bool __memuncached_set(memuncached_client_t* client, char* key, char type, void* data, int length)
{
    if (type < MEMCACHED_NUMBER_RESULT_DECIMAL || type > MEMCACHED_NUMBER_RESULT_STRING)
        return false;

    dprintf(client->fd, "SET %s %d %d\r\n", key, type, length);

    if (type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
        dprintf(client->fd, "%ld\r\n", *((int64_t*)data));
    } else if (type == MEMCACHED_NUMBER_RESULT_REAL) {
        dprintf(client->fd, "%lf\r\n", *((double*)data));
    } else if (type == MEMCACHED_NUMBER_RESULT_STRING) {
        write(client->fd, (char*)data, length);
        write(client->fd, "\r\n", 2);
    }

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    return is_success;
}

bool __memuncached_add(memuncached_client_t* client, char* key, char type, void* data, int length)
{
    if (type < MEMCACHED_NUMBER_RESULT_DECIMAL || type > MEMCACHED_NUMBER_RESULT_STRING)
        return false;

    dprintf(client->fd, "ADD %s %d %d\r\n", key, type, length);

    if (type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
        dprintf(client->fd, "%ld\r\n", *((int64_t*)data));
    } else if (type == MEMCACHED_NUMBER_RESULT_REAL) {
        dprintf(client->fd, "%lf\r\n", *((double*)data));
    } else if (type == MEMCACHED_NUMBER_RESULT_STRING) {
        write(client->fd, (char*)data, length);
        write(client->fd, "\r\n", 2);
    }

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    return is_success;
}

bool memuncached_disconnect(memuncached_client_t* client)
{
    dprintf(client->fd, "BYE\r\n");

    bool is_success = memuncached_recv(client);
    is_success = is_success && client->response.code == 200;

    if (is_success) {
        LOG_DEBUG("DISCONNECT.Response.code: '%d'", client->response.code);
        LOG_DEBUG("DISCONNECT.Response.description: '%s'", client->response.description);
    }
    LOG_DEBUG("is_success: %d", is_success);

    int result = shutdown(client->fd, SHUT_RDWR) + close(client->fd);
    free(client->buffer);
    free(client);
    LOG_DEBUG("result: %d", result);

    return (result == 0 && is_success);
}

bool memuncached_value_result_clean(memuncached_value_result_t* result)
{
    if (result == NULL)
        return false;

    if (result->type != MEMCACHED_NUMBER_RESULT_STRING)
        return false;

    if (result->string == NULL)
        return false;

    free(result->string);

    return true;
}
