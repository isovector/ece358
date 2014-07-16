#include "rcs.h"
#include "rcs_t.h"
#include <iostream>

#include <string>

extern int errno;

#define SOCKET (rcs_t::getSocket(sockfd))
#define CHECK_SOCKET if (!SOCKET) { return -1; }

//  Description:
//    Used to allocated an RCS socket.
//  Returns:
//      int : socket descriptor (positive integer) on success
int rcsSocket() {
    return rcs_t::makeSocket();
}

//  Description:
//    Binds an RCS socket to the address structure
//  Input:
//    int sockfd : socket descriptor
//    const struct sockaddr_in *addr : address structure to bind socket to
//  Output:
//    int : error code
int rcsBind(int sockfd, struct sockaddr_in *addr) {
    CHECK_SOCKET;
    return SOCKET->bind(addr);
}

//  Description:
//    Fills provided address structure with address that socket has been bound
//  Input:
//    int sockfd : socket descriptor
//    struct sockaddr_in *addr : address structure to be filled with socket address info
//  Output:
//    int : error code
int rcsGetSockName(int sockfd, struct sockaddr_in *addr) {
    CHECK_SOCKET;
    return SOCKET->getSockName(addr);
}

//  Description:
//    Marks an RCS socket as listening for connection requests.
//  Input:
//    int sockfd : socket descriptor
//  Output:
//    int : error code
int rcsListen(int sockfd) {
    CHECK_SOCKET;
    SOCKET->listen();
    return 0;
}

//  Description:
//    Accepts a connection request on the socket.
//    This is a blocking call.
//  Input:
//    int sockfd : socket descriptor
//    struct sockaddr_in *addr : filled with address of requester
//  Output:
//      int : socket descriptor of newly created socket for
//             communicating with requester
int rcsAccept(int sockfd, struct sockaddr_in *addr) {
    CHECK_SOCKET;
    return SOCKET->accept(addr);
}

//  Description:
//    Connects a client to a server
//  Input:
//    int sockfd : socket descriptor (socket must be bound beforehand)
//    const struct sockaddr_in *addr: address of the server
//  Output:
//    int : error code
int rcsConnect(int sockfd, const struct sockaddr_in *addr) {
    CHECK_SOCKET;
    return SOCKET->connect(addr);
}

//  Description:
//    Blocking receive.
//  Input:
//    int sockfd : socket descriptor
//    void *data : buffer that received data is put in to
//    int size   : expected amount of data (in bytes)
int rcsRecv(int sockfd, void *data, int size) {
    CHECK_SOCKET;
    return SOCKET->recv(
        static_cast<char*>(data),
        static_cast<size_t>(size)
    );
}

//  Description:
//    Blocking send.
//  Input:
//    int sockfd : socket descriptor
//    const void *data : buffer containing data to be sent
//    int size         : amount of data to be sent (in bytes)
int rcsSend(int sockfd, const void *data, int size) {
    CHECK_SOCKET;
    return SOCKET->send(
        static_cast<const char*>(data),
        static_cast<size_t>(size)
    );
}

//  Description:
//    Closes an RCS socket
//  Input:
//    int sockfd : socket descriptor
//  Output:
//    int : error code
int rcsClose(int sockfd) {
    CHECK_SOCKET;
    rcs_t::destroySocket(sockfd);
    return 0;
}

