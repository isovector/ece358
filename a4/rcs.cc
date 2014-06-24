#include "rcs.h"
#include "rcs_socket.h"

#include <string>

extern int errno;

#define SOCKET (rcs_t::getSocket(sockfd))

int rcsSocket() {
    return rcs_t::makeSocket();
}

int rcsBind(int sockfd, const struct sockaddr_in *addr) {
    return SOCKET.bind(addr);
}

int rcsGetSockName(int sockfd, struct sockaddr_in *addr) {
    return ucpGetSockName(SOCKET.getUcpSocket(), addr);
}

int rcsListen(int sockfd) {
    ucpSetSockRecvTimeout(SOCKET.getUcpSocket(), 10);
    SOCKET.markAsListenerSocket();
    return 0;
}

int rcsAccept(int sockfd, struct sockaddr_in *addr) {
    if(SOCKET.isListenerSocket()) {
        const int LENGTH_OF_SYN = 8;
        char buffer[LENGTH_OF_SYN];
        int bytes_received = SOCKET.recv(buffer, LENGTH_OF_SYN);
        buffer[bytes_received] = 0;
        std::string syn = buffer;
        if(syn == "Connection Please") {
            return rcs_t::makeSocket();
        }
        else {
            return -1;
        }
        // if message is valid, then SYN+ACK
        // else fail or re-request
    }
    else {
        return -1;
    }

    return 0;
}

int rcsConnect(int sockfd, const struct sockaddr_in *addr) {
    return SOCKET.connect(addr);
}

int rcsRecv(int sockfd, void *data, int size) {
    return SOCKET.recv(
        static_cast<char*>(data),
        static_cast<size_t>(size)
    );
}

int rcsSend(int sockfd, const void *data, int size) {
    return SOCKET.send(
        static_cast<const char*>(data),
        static_cast<size_t>(size)
    );
}

int rcsClose(int sockfd) {
    rcs_t::destroySocket(sockfd);
    return 0;
}

