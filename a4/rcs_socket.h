#pragma once

#include "ucp_c.h"
#include "message.h"
#include <map>
#include "msg_buffer.h"

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
    int send(const char *data, size_t length);
    int recv(char *data, size_t maxLength);

    int getUcpSocket() const;
    void markAsListenerSocket();
    bool isListenerSocket() const;

private:
    void acksend(const msg_t &msg) const;
    int rawsend(const msg_t &msg) const;
    bool rawrecv(msg_t *out) const;
    bool poll() const;

    int ucpSocket_;
    bool isListenerSocket_;
    sockaddr_in endPoint_;
    msg_buffer_t buffer_;

    uint16_t sendSeqnum_;
    uint16_t recvSeqnum_;
};

