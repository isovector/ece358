#include "rcs.h"
#include "rcs_socket.h"

extern int errno;

#define SOCKET (rcs_t::getSocket(sockfd))

int rcsSocket() {
    return rcs_t::makeSocket();
}

int rcsBind(int sockfd, const struct sockaddr_in *addr) {
    return SOCKET.bind(addr);
}

int rcsGetSockName(int sockfd, struct sockaddr_in *addr) {
    return 0;
}

int rcsListen(int sockfd) {
    return 0;
}

int rcsAccept(int sockfd, struct sockaddr_in *addr) {
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

