#include "rcs_socket.h"
#include <string.h>
#include <iostream>
using namespace std;

rcs_t::sockets_t rcs_t::sSocketIdentifiers;

rcs_t &rcs_t::getSocket(int id) {
    sockets_t::iterator it = sSocketIdentifiers.find(id);
    if (it == sSocketIdentifiers.end()) {
        // make this a proper exception
        throw "BAD SOCKET, SUCKA";
    }

    return it->second;
}

int rcs_t::makeSocket() {
    static size_t sNextId = 0;

    sSocketIdentifiers[sNextId];
    return sNextId++;
}

void rcs_t::destroySocket(int id) {
    sockets_t::iterator it = sSocketIdentifiers.find(id);
    if (it == sSocketIdentifiers.end()) {
        // make this a proper exception
        throw "BAD SOCKET, SUCKA";
    }

    sSocketIdentifiers.erase(it);
}



rcs_t::rcs_t() : 
    ucpSocket_(ucpSocket())
{
}

int rcs_t::bind(const sockaddr_in *addr) {
    return ucpBind(ucpSocket_, addr);
}

int rcs_t::connect(const sockaddr_in *addr) {
    memcpy(&endPoint_, addr, sizeof(sockaddr_in));
    //TODO: handshake here
    return 0;
}

int rcs_t::send(const char *data, size_t length) const {
    //TODO: error recovery
    return ucpSendTo(
        ucpSocket_, 
        static_cast<const void*>(data), 
        static_cast<int>(length),
        &endPoint_
    );
}

int rcs_t::recv(char *data, size_t maxLength) const {
    sockaddr_in unused;
    //TODO: error recovery
    return ucpRecvFrom(
        ucpSocket_, 
        static_cast<void*>(data), 
        static_cast<int>(maxLength),
        &unused
    );
}
