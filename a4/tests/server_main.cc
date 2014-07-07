#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

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
    rcsBind(socket, &addr);

    return socket;
}

int main() {
    static const short PORT = 6112;
    int listen = newSocket(PORT);

    rcsListen(listen);
    int client = rcsAccept(listen, NULL);

    const size_t LENGTH = 10000000;
    char *buffer = new char[LENGTH];
    ifstream file("send.mp3", ios::in | ios::binary);

    size_t length = file.readsome(buffer, LENGTH);
    rcsSend(client, buffer, length);

    rcsClose(client);
    rcsClose(listen);

    delete buffer;
}

