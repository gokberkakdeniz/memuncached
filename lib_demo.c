#include "libmemuncached.h"
#include "logger.h"

int main(int argc, char const* argv[])
{
    memuncached_client_t* client = memuncached_connect("127.0.0.1", 9999, "", "");

    if (client == NULL) {
        printf("connection failed.");
        return 1;
    }

    double set_f_data = 5.999874545;
    if (memuncached_set_f(client, "real", (void*)&set_f_data)) {
        printf("memuncached_set_f('real', %lf): true\n", set_f_data);
    } else {
        printf("memuncached_set_f('real', %lf): false\n", set_f_data);
    }
    int64_t set_d_data = -123456;
    if (memuncached_set_d(client, "decimal", (void*)&set_d_data)) {
        printf("memuncached_set_d('decimal', %ld): true\n", set_d_data);
    } else {
        printf("memuncached_set_d('decimal', %ld): false\n", set_d_data);
    }
    char set_s_data[] = "Lorem ipsum sit amet dolor.";
    if (memuncached_set_d(client, "text", (void*)&set_d_data)) {
        printf("memuncached_set_s('text', '%s'): true\n", set_s_data);
    } else {
        printf("memuncached_set_s('text', '%s'): false\n", set_s_data);
    }
    char add_s_data[] = "hello world.";
    if (memuncached_add_s(client, "text", (void*)&add_s_data, 12)) {
        printf("memuncached_add_s('text', '%s'): true\n", add_s_data);
    } else {
        printf("memuncached_add_s('text', '%s'): false\n", add_s_data);
    }

    memuncached_value_result_t res_get = {};
    if (memuncached_get(client, "real", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_get('real'): %d\n", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_get('real'): %f\n", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            printf("memuncached_get('real') >>>\n");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("memuncached_get('real') <<<\n");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        printf("memuncached_get('real'): false\n");
    }
    if (memuncached_get(client, "text", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_get('text'): %d\n", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_get('text'): %f\n", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            printf("memuncached_get('text') <<<\n");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("memuncached_get('text') >>>\n");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        printf("memuncached_get('text'): false\n");
    }
    if (memuncached_get(client, "decimal", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_get('decimal'): %d\n", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_get('decimal'): %f\n", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            printf("memuncached_get('decimal') <<<\n");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("memuncached_get('decimal') >>>\n");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        printf("memuncached_get('decimal'): false\n");
    }
    if (memuncached_get(client, "x", &res_get)) {
        if (res_get.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_get('x'): %d\n", res_get.decimal);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_get('x'): %f\n", res_get.real);
        } else if (res_get.type == MEMCACHED_NUMBER_RESULT_STRING) {
            printf("memuncached_get('x') <<<\n");
            for (size_t i = 0; i < res_get.size; i++) {
                printf("%c", res_get.string[i]);
            }
            printf("memuncached_get('x') >>>\n");
        }
        memuncached_value_result_clean(&res_get);
    } else {
        printf("memuncached_get('x'): false\n");
    }

    memuncached_stt_result_t res_stt = {};
    if (memuncached_stt(client, &res_stt)) {
        printf("memuncached_stt(): key_count: %d, client_count: %d\n", res_stt.key_count, res_stt.client_count);
    }

    memuncached_ver_result_t res_ver = {};
    if (memuncached_ver(client, &res_ver)) {
        printf("memuncached_ver(): %s\n", res_ver.version);
    }

    memuncached_value_result_t res_inc = {};
    if (memuncached_inc_f(client, "x", &res_inc)) {
        if (res_inc.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_inc_f('x'): %ld\n", res_inc.decimal);
        } else if (res_inc.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_inc_f('x'): %lf\n", res_inc.real);
        }
    }

    memuncached_value_result_t res_dec = {};
    if (memuncached_dec_d(client, "x", &res_dec)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_dec_d('x'): %ld\n", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_dec_d('x'): %lf\n", res_dec.real);
        }
    }

    if (memuncached_dec_d(client, "x", &res_dec, 10)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_dec_d('x', 10): %d\n", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_dec_d('x', 10): %lf\n", res_dec.real);
        }
    }

    memuncached_value_result_t res_del = {};
    if (memuncached_del(client, "x", &res_del)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_del('x'): %ld\n", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_del('x'): %lf\n", res_dec.real);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_STRING) {
            printf("memuncached_del('x') <<<\n");
            for (size_t i = 0; i < res_del.size; i++) {
                printf("%c", res_del.string[i]);
            }
            printf("memuncached_del('x') >>>\n");
        }
        memuncached_value_result_clean(&res_del);
    }

    if (memuncached_dec_d(client, "x", &res_dec, 10, 19)) {
        if (res_dec.type == MEMCACHED_NUMBER_RESULT_DECIMAL) {
            printf("memuncached_dec_d('x', 10, 19): %d\n", res_dec.decimal);
        } else if (res_dec.type == MEMCACHED_NUMBER_RESULT_REAL) {
            printf("memuncached_dec_d('x', 10, 19): %lf\n", res_dec.real);
        }
    }

    if (memuncached_disconnect(client)) {
        printf("memuncached_disconnect(): %s\n", "success");
    } else {
        printf("memuncached_disconnect(): %s\n", "fail");
    }

    return 0;
}