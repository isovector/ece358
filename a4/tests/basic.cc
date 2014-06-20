#include "harness.h"
#include "../rcs.h"
#include <string.h>
#include <string>

sockaddr_in getAddr(short port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);
	return addr;
}

int newSocket(short port) {
	int socket = rcs_t::makeSocket();
	sockaddr_in addr = getAddr(port);
    int result = rcsBind(socket, &addr);
    EXPECT(result >= 0);

    return socket;
}



UNIT_TEST(BasicSendRecv) {
}

// Ensure the testing framework works as expected
UNIT_TEST(UnitTestHarnessTest) {
    EXPECT(true);
    EXPECT(!false);
}

