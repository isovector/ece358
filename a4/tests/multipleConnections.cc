#include "harness.h"
#include "../rcs.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdlib>

#include <vector>

using namespace std;

extern int newSocket(short port);
extern sockaddr_in getAddr(short port);

UNIT_TEST(MultipleConnections) {
	static const short SERVER_PORT = 9899;
	static const short CLIENT1_PORT = 9900;
	static const short CLIENT2_PORT = 9901;

	sockaddr_in serverAddr = getAddr(SERVER_PORT);

	int server = newSocket(SERVER_PORT);
	int client1 = newSocket(CLIENT1_PORT);
	int client2 = newSocket(CLIENT2_PORT);

	EXPECT(server != client1);
	EXPECT(client1 != client2);
	EXPECT(server >= 0);
	EXPECT(client1 >= 0);
	EXPECT(client2 >= 0);

	EXPECT(0 == rcsListen(server));

    string data = "the quick brown fox jumps OVER the lazy dawgggg";
    int data_len = data.length();

	if(!fork()) {

		// Two clients sending data
		if (!fork())
		{
			EXPECT(0 == rcsConnect(client1, &serverAddr));
			// EXPECT_N(data_len, rcsSend(client1, data.c_str(), data_len));
			_exit(0);
		} else {
			EXPECT(0 == rcsConnect(client2, &serverAddr));
			// EXPECT_N(data_len, rcsSend(client2, data.c_str(), data_len));
			_exit(0);
		}

	} else {
		// server
		int num_clients = 2;
		sockaddr_in *receivedFrom;
		vector<int> clients;
		for (int i = 0; i < num_clients; i++) {
			int client = rcsAccept(server, receivedFrom);
			EXPECT( -1 != client );

			cout << "Latest client: " << client << endl;
			clients.push_back( client );
			cout << "Number of clients: " << clients.size() << endl;
		}
	}
}

