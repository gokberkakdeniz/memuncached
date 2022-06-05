#include "libmemuncached.h"
#include "logger.h"

int main(int argc, char const* argv[])
{
    memuncached_client_t* client = memuncached_connect("127.0.0.1", 9999, "", "");

    if (client == NULL) {
        LOG_FATAL("connection failed.")
        return 1;
    }

    memuncached_value_result_t res_get = {};
    if (memuncached_get(client, "real", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("get.decimal: %d", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("get.real: %f", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            LOG_INFO("get.string:<<<");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            LOG_INFO("get.string:>>>");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        LOG_INFO("get false");
    }
    if (memuncached_get(client, "text", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("get.decimal: %d", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("get.real: %f", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            LOG_INFO("get.string:<<<");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("\n");
            LOG_INFO("get.string:>>>");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        LOG_INFO("get false");
    }
    if (memuncached_get(client, "x", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("get.decimal: %d", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("get.real: %f", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            LOG_INFO("get.string:<<<");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("\n");
            LOG_INFO("get.string:>>>");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        LOG_INFO("get false");
    }

    memuncached_stt_result_t res_stt = {};
    if (memuncached_stt(client, &res_stt)) {
        LOG_INFO("key_count: %d", res_stt.key_count);
        LOG_INFO("client_count: %d", res_stt.client_count);
    }

    memuncached_ver_result_t res_ver = {};
    if (memuncached_ver(client, &res_ver)) {
        LOG_INFO("version: %s", res_ver.version);
    }

    memuncached_value_result_t res_inc = {};
    if (memuncached_inc_f(client, "x", &res_inc)) {
        if (res_inc.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("inc.decimal: %d", res_inc.decimal);
        } else if (res_inc.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("inc.real: %f", res_inc.real);
        }
    }

    memuncached_value_result_t res_dec = {};
    if (memuncached_dec_d(client, "x", &res_dec)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("dec.decimal: %d", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("dec.real: %f", res_dec.real);
        }
    }

    if (memuncached_dec_d(client, "x", &res_dec, 10)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("dec.decimal: %d", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("dec.real: %f", res_dec.real);
        }
    }

    memuncached_value_result_t res_del = {};
    if (memuncached_del(client, "x", &res_del)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("del.decimal: %d", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("del.real: %f", res_dec.real);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_STRING) {
            LOG_INFO("del.string:<<<");
            for (size_t i = 0; i < res_del.size; i++) {
                printf("%c", res_del.string[i]);
            }
            LOG_INFO("del.string:>>>");
        }
        memuncached_value_result_clean(&res_del);
    }

    if (memuncached_dec_d(client, "x", &res_dec, 10, 19)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            LOG_INFO("dec.decimal: %d", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            LOG_INFO("dec.real: %f", res_dec.real);
        }
    }

    if (memuncached_disconnect(client)) {
        LOG_INFO("disconnect: %s", "success");
    } else {
        LOG_INFO("disconnect: %s", "fail");
    }

    return 0;
}