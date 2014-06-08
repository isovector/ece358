#include "harness.h"
#include <cstdlib>
#include <string.h>

#include "../ucp_c.h"

int createSocket(short port, sockaddr_in &dest) {
    int sock = ucpSocket();

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dest.sin_port = htons(port);

    EXPECT(ucpBind(sock, &dest) >= 0);

    return sock;
}


UNIT_TEST(BasicRDPTest) {
    sockaddr_in client_ep, server_ep;

    int server = createSocket(6666, server_ep);
    int client = createSocket(6667, client_ep);

    EXPECT(server > 0);
    EXPECT(client > 0);
    EXPECT(server != client);

    ucpClose(server);
    ucpClose(client);
}

