#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

extern int newSocket(short port);
extern sockaddr_in getAddr(short port);

UNIT_TEST(GetSocketName) {
    static const short PORT = 6413;
    int sock = newSocket(PORT);

    EXPECT(sock >= 0);

    struct sockaddr_in sock_addr;
    int result = rcsGetSockName(sock, &sock_addr);
    EXPECT(result == 0);

    EXPECT(sock_addr.sin_addr.s_addr == inet_addr("127.0.0.1"));
    // this should probably be ntohs on the sock_addr
    EXPECT(sock_addr.sin_port == ntohs(6413));
}
