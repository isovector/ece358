#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

extern bool testHasFailed;

sockaddr_in getAddr(short port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);
	return addr;
}

int newSocket(short port) {
	int socket = rcsSocket();
	sockaddr_in addr = getAddr(port);
    int result = rcsBind(socket, &addr);
    EXPECT(result >= 0);

    return socket;
}

UNIT_TEST(BasicSendRecv) {
    static const short SERVER_PORT = 6141;
    static const short CLIENT_PORT = 6142;

    sockaddr_in serverAddr = getAddr(SERVER_PORT);
    sockaddr_in clientAddr = getAddr(CLIENT_PORT);

    int server = newSocket(SERVER_PORT);
    int client = newSocket(CLIENT_PORT);

    EXPECT(server != client);
    EXPECT(server >= 0);
    EXPECT(client >= 0);

    EXPECT(0 == rcsListen(server));

    //TODO: in the future this should listen or something
    EXPECT(0 == rcsConnect(client, &serverAddr));
    EXPECT(0 == rcsConnect(server, &clientAddr));

    string data = "the quick brown fox jumps OVER the lazy dawgggg";
    int len = (int)data.length();

    if (!fork()) {
        EXPECT_N(len, rcsSend(server, data.c_str(), len));
        _exit(0);
    } else {
        char buffer[1024];
        EXPECT_N(len, rcsRecv(client, buffer, len));
        EXPECT(data == buffer);
    }

    rcsClose(server);
    rcsClose(client);
}

UNIT_TEST(GetSocketName) {
    static const short PORT = 6413;
    sockaddr_in addr = getAddr(PORT);
    int sock = newSocket(PORT);

    EXPECT(sock >= 0);

    sockaddr_in sock_addr;
    int result = rcsGetSockName(sock, &sock_addr);

    EXPECT(result == 0);
    // cout << sock_addr.sin_port << endl;
    // EXPECT(sock_addr.sin_port == 6413);
}

