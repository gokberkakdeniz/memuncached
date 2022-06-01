#include "libmemuncached.h"
#include "logger.h"

int main(int argc, char const* argv[])
{
    memuncached_client_t* client = memuncached_connect("127.0.0.1", 9999, "", "");

    if (client == NULL) {
        LOG_FATAL("connection failed.")
        return 1;
    }

    memuncached_stt_result_t res_stt = {};
    if (memuncached_stt(client, &res_stt)) {
        LOG_INFO("kc: %d", res_stt.key_count);
        LOG_INFO("cc: %d", res_stt.client_count);
    }

    memuncached_ver_result_t res_ver = {};
    if (memuncached_ver(client, &res_ver)) {
        LOG_INFO("ver: %s", res_ver.version);
    }
    // memuncached_inc(client, "x");
    // memuncached_dec(client, "x", 10);
    // memuncached_del(client, "x");
    // memuncached_dec(client, "x", 10, 10);

    memuncached_disconnect(client);

    return 0;
}