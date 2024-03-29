#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdlib>

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
    static const short SERVER_PORT = 6112;
    static const short CLIENT_PORT = 6111;

    sockaddr_in serverAddr = getAddr(SERVER_PORT);

    int server = newSocket(SERVER_PORT);
    int client = newSocket(CLIENT_PORT);

    EXPECT(server != client);
    EXPECT(server >= 0);
    EXPECT(client >= 0);

    EXPECT(0 == rcsListen(server));

    string data = "the quick brown fox jumps OVER the lazy dawgggg";
    int len = (int)data.length();

    if (!fork()) {
        int channel = rcsAccept(server, NULL);
        cout << "------ ACCEPTED" << endl;
        EXPECT_N(len, rcsSend(channel, data.c_str(), len));
        _exit(0);
    } else {
        EXPECT(0 == rcsConnect(client, &serverAddr));
        cout << "------ CONNECTED" << endl;

        char buffer[1024];
        EXPECT_N(len, rcsRecv(client, buffer, len));
        EXPECT(data == buffer);
    }

    rcsClose(server);
    rcsClose(client);
}
