#include "libmemuncached.h"
#include "logger.h"

int main(int argc, char const* argv[])
{
    memuncached_client_t* client = memuncached_connect("127.0.0.1", 9999, "", "");

    if (client == NULL) {
        return 1;
    }

    memuncached_stt(client);

    // memuncached_ver(client);

    // memuncached_inc(client, "x");
    // memuncached_dec(client, "x", 10);
    // memuncached_del(client, "x");
    // memuncached_dec(client, "x", 10, 10);

    memuncached_disconnect(client);

    return 0;
}