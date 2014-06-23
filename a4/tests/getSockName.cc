#include "harness.h"
#include "../rcs.h"
// #include <string>
// #include <string.h>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

sockaddr_in getAddrNow(short port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);
	return addr;
}

int newSocketMaker(short port) {
	int socket = rcsSocket();
	sockaddr_in addr = getAddrNow(port);
    int result = rcsBind(socket, &addr);
    EXPECT(result >= 0);

    return socket;
}

UNIT_TEST(GetSocketName) {
    static const short PORT = 6413;
    int sock = newSocketMaker(PORT);

    EXPECT(sock >= 0);

    struct sockaddr_in sock_addr;
    int result = rcsGetSockName(sock, &sock_addr);

    EXPECT(result == 0);
    // cout << sock_addr.sin_port << endl;
    // EXPECT(sock_addr.sin_port == 6413);
}