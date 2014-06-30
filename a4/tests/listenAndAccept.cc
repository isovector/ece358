#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdlib>

using namespace std;

extern int newSocket(short port);
extern sockaddr_in getAddr(short port);

UNIT_TEST(Listener) {
	static const short SERVER_PORT = 9851;
	static const short CLIENT_PORT = 9852;

	sockaddr_in serverAddr = getAddr(SERVER_PORT);

	int server = newSocket(SERVER_PORT);
	int client = newSocket(CLIENT_PORT);

	EXPECT(server != client);
	EXPECT(server >= 0);
	EXPECT(client >= 0);

	EXPECT(0 == rcsListen(server));

	EXPECT(0 == rcsConnect(client, &serverAddr));

	if(!fork()) {
		// client
		string handshake = "Connection Please";
		int len = (int)handshake.length();
		EXPECT_N(len, rcsSend(client, handshake.c_str(), len ));
		_exit(0);
	} else {
		// server
		sockaddr_in *receivedFrom;
		EXPECT( -1 != rcsAccept(server, receivedFrom));
	}
}

