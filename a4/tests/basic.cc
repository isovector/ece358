#include "harness.h"
#include "../rcs.h"
#include <string.h>

int newSocket(short port) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return 0;
}

UNIT_TEST(BasicSendRecv) {
}

// Ensure the testing framework works as expected
UNIT_TEST(UnitTestHarnessTest) {
    EXPECT(true);
    EXPECT(!false);
}

