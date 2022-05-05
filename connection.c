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
                    // TODO: send error
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

        if (offset != NULL && !is_numeric_string(offset)) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEC", "The argument OFFSET must be decimal.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (initial != NULL && !is_numeric_string(initial)) {
            REPLY_BAD_REQUEST(client->socket_fd, "DEC", "The argument INITIAL must be decimal.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_dec(client, key, offset, initial);
    } else if (strcasecmp(cmd, "DEL") == 0) {

    } else if (strcasecmp(cmd, "INC") == 0) {
        char* key = GET_NEXT_ARG(itr);
        char* offset = GET_NEXT_ARG(itr);
        char* initial = GET_NEXT_ARG(itr);

        LOG_DEBUG("key=%s, offset=%s, initial=%s", key, offset, initial);

        if (key == NULL) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument KEY is mandatory.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (offset != NULL && !is_numeric_string(offset)) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument OFFSET must be decimal.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        if (initial != NULL && !is_numeric_string(initial)) {
            REPLY_BAD_REQUEST(client->socket_fd, "INC", "The argument INITIAL must be decimal.", command_escaped);
            HANDLE_COMMAND_DONE;
        }

        memuncached_inc(client, key, offset, initial);
    } else if (strcasecmp(cmd, "GET") == 0) {

    } else if (strcasecmp(cmd, "SET") == 0) {

    } else if (strcasecmp(cmd, "STT") == 0) {
        if (*itr != 0) {
            LOG_INFO("^%c^", *itr);
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

    LOG_DEBUG("215");

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
    REPLY_SUCCESS(client->socket_fd, "Client-Count: %d\r\nKey-Count: %d", clients->count, table->count);
}

void memuncached_ver(client_connection_t* client)
{
    REPLY_SUCCESS(client->socket_fd, MEMUNCACHED_VERSION);
}

void memuncached_dec(client_connection_t* client, char* key, char* offset, char* initial)
{
}

void memuncached_inc(client_connection_t* client, char* key, char* offset, char* initial)
{
    LOG_DEBUG("memuncached_inc");
}