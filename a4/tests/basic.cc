#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
using namespace std;


int newSocket(short port) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return rcsSocket(); 
}

sockaddr_in getAddr(short port) {
}

UNIT_TEST(BasicSendRecv) {
    static const short SERVER_PORT = 6111;
    static const short CLIENT_PORT = 6112;

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

    string data = "hello thug life how are you today?";
    size_t len = data.length();

    EXPECT_N(len, rcsSend(server, data.c_str(), len));
}

