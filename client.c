#include "libmemuncached.h"
#include "logger.h"

int main(int argc, char const* argv[])
{
    memuncached_client_t* client = memuncached_connect("127.0.0.1", 9999, "", "");

    memuncached_stt(client);

    memuncached_disconnect(client);

    return 0;
}
