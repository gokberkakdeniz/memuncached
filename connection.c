#include "connection.h"
#include "vector.h"

extern hash_table_t* table;
extern vector_t* clients;

#define HANDLE_COMMAND_DONE goto handle_command_done;
#define GET_NEXT_ARG(itr)   \
    *itr == 0 ? NULL : itr; \
    itr = put_null_terminator_unknown_size(itr);

void* handle_connection(void* arg)
{
    client_connection_t* client = (client_connection_t*)arg;
    client->thread_id = pthread_self();
    pthread_detach(client->thread_id);

    LOG_INFO(LOG_CLIENT_FORMAT "thread started.", LOG_CLIENT_FORMAT_ARGS);

    bool is_end;
    ssize_t read_size;
    size_t buffer_size = CONNECTION_RECV_LENGTH;
    char buffer[buffer_size];
    char* command = NULL;
    size_t command_len = 0;
    ssize_t command_free_len = 0;

    write(client->socket_fd, CLIENT_WELCOME_MESSAGE, CLIENT_WELCOME_MESSAGE_LEN);

    while (is_running && client->is_thread_running) {
        is_end = false;
        command_len = CONNECTION_RECV_LENGTH + 1;
        command_free_len = command_len;
        command = (char*)calloc(command_len, sizeof(char));

        while (is_running && !is_end && (read_size = recv(client->socket_fd, buffer, buffer_size, 0)) > 0) {
            if (read_size == 0) {
                LOG_INFO(LOG_CLIENT_FORMAT "client disconnected.", LOG_CLIENT_FORMAT_ARGS);
                fflush(stdout);
                goto clean_and_stop;
            } else if (read_size == -1) {
                LOG_ERROR(LOG_CLIENT_FORMAT "recv failed. " LOG_CLIENT_ERROR_FORMAT, LOG_CLIENT_FORMAT_ARGS, LOG_CLIENT_ERROR_FORMAT_ARGS);
                goto clean_and_stop;
            }

            buffer[read_size] = '\0';
            is_end = buffer[read_size - 1] == '\n';

            command_free_len = command_free_len - (read_size + 1);
            if (command_free_len < 0) {
                command_free_len = command_len - (read_size + 1);
                command_len *= 2;
                char* command_new = (char*)realloc(command, command_len);

                if (command_new == NULL) {
                    LOG_ERROR(LOG_CLIENT_FORMAT "could not realloc for command. " LOG_CLIENT_ERROR_FORMAT, LOG_CLIENT_FORMAT_ARGS, LOG_CLIENT_ERROR_FORMAT_ARGS);
                    REPLY_SERVER_ERROR(client->socket_fd, "Realloc failed.");
                    goto clean_and_continue;
                } else {
                    command = command_new;
                }
            }

            strcat(command, buffer);

            memset(buffer, 0, buffer_size);
        }

        if (!is_end) {
            goto clean_and_continue;
        }

        command_len = strlen(command);
        while (command[command_len - 1] == '\r' || command[command_len - 1] == '\n') {
            command[--command_len] = 0;
        }

        handle_command(command, client);

    clean_and_continue:
        free(command);
        continue;

    clean_and_stop:
        free(command);
        client->is_thread_running = false;
    }

    // TODO: test if vector_lock works...
    if (is_running && vector_timed_lock(clients, 1) == 0) {
        for (size_t i = 0; i < clients->count; i++) {
            client_connection_t* c = (client_connection_t*)vector_at_unsafe(clients, i);
            if (c->thread_id == client->thread_id) {
                vector_remove_unsafe(clients, i);
                break;
            }
        }

        close(client->socket_fd);
        LOG_INFO(LOG_CLIENT_FORMAT "cleaned.", LOG_CLIENT_FORMAT_ARGS);
        free(client);
        vector_unlock(clients);
    }

    return NULL;
}

char* put_null_terminator_unknown_size(char* itr)
{
    while (*itr != 0 && *itr != ' ' && *itr != '\r' && *itr != '\n') {
        ++itr;
    }

    while (*itr == ' ' || *itr == '\r' || *itr == '\n') {
        *itr = 0;
        ++itr;
    }

    return itr;
}

void handle_command(const char* command, client_connection_t* client)
{
    char* command_escaped = str_escape(command);
    LOG_INFO(LOG_CLIENT_FORMAT "Query received: '%s'", LOG_CLIENT_FORMAT_ARGS, command_escaped);

    char* itr = (char*)command;

    char* cmd = GET_NEXT_ARG(itr);

    if (cmd == NULL) {
        HANDLE_COMMAND_DONE;
    }

    if (strcasecmp(cmd, "ADD") == 0) {
        char* key = GET_NEXT_ARG(itr);
        char* type = GET_NEXT_ARG(itr);
        char* length = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s, type=%s, length=%s", key, type, length);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "ADD", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (type == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "ADD", "The argument TYPE is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (*type == '-' || !is_decimal_string(type)) {
            REPLY_BAD_REQUEST(client->socket_fd, "ADD", "The argument TYPE must be positive integer.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (length == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "ADD", "The argument LENGTH is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (*length == '-' || !is_decimal_string(length)) {
            REPLY_BAD_REQUEST(client->socket_fd, "ADD", "The argument LENGTH must be positive integer.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_add(client, key, type, length);
    } else if (strcasecmp(cmd, "BYE") == 0) {
        if (*itr != 0) {
            REPLY_BAD_REQUEST(client->socket_fd, "BYE", "The command does not take arguments.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_bye(client);
    } else if (strcasecmp(cmd, "DEC") == 0) {
        char* key = GET_NEXT_ARG(itr);
        char* offset = GET_NEXT_ARG(itr);
        char* initial = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s, offset=%s, initial=%s", key, offset, initial);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEC", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (offset != NULL && (*offset == '-' || !is_real_string(offset))) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEC", "The argument OFFSET must be positive real number.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (initial != NULL && !is_real_string(initial)) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEC", "The argument INITIAL must be real number.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_dec(client, key, offset, initial);
    } else if (strcasecmp(cmd, "DEL") == 0) {
        char* key = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s", key);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEL", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_del(client, key);
    } else if (strcasecmp(cmd, "INC") == 0) {
        char* key = GET_NEXT_ARG(itr);
        char* offset = GET_NEXT_ARG(itr);
        char* initial = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s, offset=%s, initial=%s", key, offset, initial);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (offset != NULL && (*offset == '-' || !is_real_string(offset))) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument OFFSET must be positive real number.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (initial != NULL && !is_real_string(initial)) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument INITIAL must be real number.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_inc(client, key, offset, initial);
    } else if (strcasecmp(cmd, "GET") == 0) {
        char* key = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s", key);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "GET", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_get(client, key);
    } else if (strcasecmp(cmd, "SET") == 0) {
        char* key = GET_NEXT_ARG(itr);
        char* type = GET_NEXT_ARG(itr);
        char* length = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s, type=%s, length=%s", key, type, length);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (type == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument TYPE is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (*type == '-' || !is_decimal_string(type)) {
            REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument TYPE must be positive integer.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (length == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument LENGTH is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (*length == '-' || !is_decimal_string(length)) {
            REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument LENGTH must be positive integer.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_set(client, key, type, length);
    } else if (strcasecmp(cmd, "STT") == 0) {
        if (*itr != 0) {
            REPLY_BAD_REQUEST(client->socket_fd, "STT", "The command does not take arguments.", command_escaped);
        } else {
            memuncached_stt(client);
        }
    } else if (strcasecmp(cmd, "VER") == 0) {
        if (*itr != 0) {
            REPLY_BAD_REQUEST(client->socket_fd, "VER", "The command does not take arguments.", command_escaped);
        } else {
            memuncached_ver(client);
        }
    } else {
        char* cmd_escaped = str_escape(cmd);
        REPLY_BAD_REQUEST(client->socket_fd, cmd_escaped, "Invalid command.", command_escaped);
        free(cmd_escaped);
    }

handle_command_done:
    free(command_escaped);
}

void memuncached_bye(client_connection_t* client)
{
    REPLY_SUCCESS(client->socket_fd, "bye.");
    client->is_thread_running = false;
}

void memuncached_stt(client_connection_t* client)
{
    REPLY_SUCCESS(client->socket_fd, "Client-Count: %d\nKey-Count: %d", clients->count, table->count);
}

void memuncached_ver(client_connection_t* client)
{
    REPLY_SUCCESS(client->socket_fd, MEMUNCACHED_VERSION);
}

void memuncached_dec(client_connection_t* client, char* key, char* offset, char* initial)
{
    cache_value_real v_offset = offset == NULL ? 1 : atof(offset);
    cache_value_t* cache_value = hash_table_dec(table, key, v_offset);

    if (cache_value == NULL) {
        cache_value_decimal initial_d = initial == NULL ? 1 : atoll(initial);
        cache_value_real initial_r = initial == NULL ? 1 : atof(initial);

        if (initial == NULL || (initial[0] != 0 && initial[1] != '.')) {
            hash_table_set(table, key, CACHE_VALUE_DECIMAL, (void*)&initial_d, 0);
        } else {
            hash_table_set(table, key, CACHE_VALUE_REAL, (void*)&initial_r, 0);
        }
    }

    memuncached_get(client, key);
}

void memuncached_inc(client_connection_t* client, char* key, char* offset, char* initial)
{
    cache_value_real v_offset = offset == NULL ? 1 : atof(offset);
    cache_value_t* cache_value = hash_table_inc(table, key, v_offset);

    if (cache_value == NULL) {
        cache_value_decimal initial_d = initial == NULL ? 1 : atoll(initial);
        cache_value_real initial_r = initial == NULL ? 1 : atof(initial);

        if (initial == NULL || (initial[0] != 0 && initial[1] != '.')) {
            hash_table_set(table, key, CACHE_VALUE_DECIMAL, (void*)&initial_d, 0);
        } else {
            hash_table_set(table, key, CACHE_VALUE_REAL, (void*)&initial_r, 0);
        }
    }

    memuncached_get(client, key);
}

void memuncached_del(client_connection_t* client, char* key)
{
    memuncached_get(client, key);
    hash_table_del(table, key);
}

void memuncached_get(client_connection_t* client, char* key)
{
    cache_value_t* cache_value = hash_table_get(table, key);

    if (cache_value == NULL) {
        REPLY_NOT_FOUND(client->socket_fd, key);
    } else {
        if (cache_value->type == CACHE_VALUE_DECIMAL) {
            int payload_len = snprintf(NULL, 0, "%ld", *(cache_value_decimal*)cache_value->value);
            dprintf(client->socket_fd, RESPONSE_200_OK "\r\n%d %d\r\n%ld%c\r\n", payload_len, cache_value->type, *(cache_value_decimal*)cache_value->value, 0);
            LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_200_OK ". Data: >>>%ld<<<", LOG_CLIENT_FORMAT_ARGS, *(cache_value_decimal*)cache_value->value);
        } else if (cache_value->type == CACHE_VALUE_REAL) {
            int payload_len = snprintf(NULL, 0, "%lf", *(cache_value_real*)cache_value->value);
            dprintf(client->socket_fd, RESPONSE_200_OK "\r\n%d %d\r\n%lf%c\r\n", payload_len, cache_value->type, *(cache_value_real*)cache_value->value, 0);
            LOG_INFO(LOG_CLIENT_FORMAT RESPONSE_200_OK ". Data: >>>%lf<<<", LOG_CLIENT_FORMAT_ARGS, *(cache_value_real*)cache_value->value);
        } else if (cache_value->type == CACHE_VALUE_STRING) {
            int payload_len = cache_value->length;
            int header_len = snprintf(NULL, 0, RESPONSE_200_OK "\r\n%d %d\r\n", payload_len, cache_value->type);
            int buffer_len = header_len + payload_len + 3;

            char* buffer = (char*)calloc(buffer_len, sizeof(char));

            LOG_DEBUG("payload_len: %d, header_len: %d, buffer_len: %d", payload_len, header_len, buffer_len);

            sprintf(buffer, RESPONSE_200_OK "\r\n%d %d\r\n", payload_len, cache_value->type);
            memcpy(buffer + header_len, cache_value->value, payload_len);
            sprintf(buffer + header_len + payload_len, "%c\r\n", 0);

            write(client->socket_fd, buffer, buffer_len);
            free(buffer);
        } else {
            REPLY_SUCCESS(client->socket_fd, "");
            LOG_ERROR("Invalid data type found deleting (key=%s, type=%c).", key, cache_value->type);
        }
    }
}

void memuncached_set(client_connection_t* client, char* key, char* type, char* length)
{
    int v_length = atol(length);
    char* payload = (char*)calloc(v_length + 2, sizeof(char));

    if (payload == NULL) {
        REPLY_SERVER_ERROR(client->socket_fd, "Calloc failed.");
    }

    int v_len = v_length + 2;
    while (v_len > 0) {
        LOG_DEBUG("v_len: %d", v_len);
        int read_size = recv(client->socket_fd, payload + v_length + 2 - v_len, v_len, 0);
        if (read_size < 0) {
            REPLY_SERVER_ERROR(client->socket_fd, "Read failed.");

            goto clean_and_stop;
        }
        v_len -= read_size;
    }

    hash_table_del(table, key);
    payload[v_length] = 0;
    payload[v_length + 1] = 0;
    LOG_DEBUG("<p>%s</p>", payload);

    cache_value_type v_type = *type - '0';
    if (v_type == CACHE_VALUE_DECIMAL) {
        cache_value_decimal v_payload = atoll(payload);
        hash_table_set(table, key, v_type, (void*)&v_payload, 0);
    } else if (v_type == CACHE_VALUE_REAL) {
        cache_value_real v_payload = atof(payload);
        hash_table_set(table, key, v_type, (void*)&v_payload, 0);
    } else if (v_type == CACHE_VALUE_STRING) {
        hash_table_set(table, key, v_type, (void*)payload, v_length);
    } else {
        REPLY_BAD_REQUEST(client->socket_fd, "SET", "The argument LENGTH must be 0, 1, or 2.", type);
        goto clean_and_stop;
    }

    REPLY_SUCCESS(client->socket_fd, "");

clean_and_stop:
    free(payload);
}

void memuncached_add(client_connection_t* client, char* key, char* type, char* length)
{
    if (hash_table_get(table, key) != NULL) {
        REPLY_KEY_EXISTS(client->socket_fd);
    } else {
        memuncached_set(client, key, type, length);
    }
}
