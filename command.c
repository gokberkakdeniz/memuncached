#include "command.h"

#define INVALID_SYNTAX(command)                    \
    {                                              \
        LOG_ERROR("Invalid syntax: %s\n", command) \
    }

void command_parse(const char* command)
{
    char cmd[11] = "";
    char* cptr = (char*)command;

    for (size_t i = 0; i < 10 && *cptr != ' ' && *cptr != 0; i++) {
        cmd[i] = *cptr;
        ++cptr;
    }

    if (*cptr == ' ')
        ++cptr;

    if (strcmp(cmd, "STAT") == 0) {
        if (*cptr != 0) {
            INVALID_SYNTAX(command);
        }

    } else if (strcmp(cmd, "VERSION") == 0) {
        if (*cptr != 0) {
            INVALID_SYNTAX(command);
        }

    } else if (strcmp(cmd, "ADD") == 0) {

    } else if (strcmp(cmd, "GET") == 0) {

    } else if (strcmp(cmd, "SET") == 0) {

    } else if (strcmp(cmd, "INC") == 0) {

    } else if (strcmp(cmd, "DEC") == 0) {

    } else if (strcmp(cmd, "DEL") == 0) {

    } else if (strcmp(cmd, "BYE") == 0) {
        if (*cptr != 0) {
            INVALID_SYNTAX(command);
        }
    }

    LOG_TRACE("cmd=%s, rest=%s\n", cmd, cptr);
}