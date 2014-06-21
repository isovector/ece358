#pragma once

#include "ucp_c.h"
#include <map>

class rcs_t {
    typedef std::map<int, rcs_t> sockets_t;
    static sockets_t sSocketIdentifiers;

public:
    static rcs_t &getSocket(int id);
    static int makeSocket();
    static void destroySocket(int id);

    rcs_t();

    int bind(const sockaddr_in *addr);
    int connect(const sockaddr_in *addr);
    int send(const char *data, size_t length) const;
    int recv(char *data, size_t maxLength) const;

private:
    int ucpSocket_;
    sockaddr_in endPoint_;
};

