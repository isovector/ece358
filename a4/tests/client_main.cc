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
    static const short PORT = 6111;
    static const short SERVER = 6112;
    int sock = newSocket(PORT);

    sockaddr_in serverAddr = getAddr(SERVER);
    rcsConnect(sock, &serverAddr);

    char buffer[1024];
    buffer[rcsRecv(sock, buffer, 1024)] = 0;

    cout << buffer << endl;

    strcpy(buffer, "hello joe");

    rcsSend(sock, buffer, strlen(buffer));
    rcsClose(sock);
}

