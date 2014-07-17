#pragma once

#include <map>

#include "ucp_c.h"
#include "msg_t.h"
#include "msg_buffer_t.h"

class rcs_t
{
    typedef std::map<int, rcs_t> sockets_t;
    static sockets_t sSocketIdentifiers;

public:
    static rcs_t *getSocket(int id);
    static int makeSocket();
    static void destroySocket(int id);

    rcs_t();

    int bind(sockaddr_in *addr);
    void listen();
    int accept(sockaddr_in *addr);
    int connect(const sockaddr_in *addr);
    int send(const char *data, size_t length);
    int recv(char *data, size_t maxLength);

    int getSockName(sockaddr_in *addr) const;

private:
    void acksend(const msg_t &msg, msg_t *resp = NULL);
    int rawsend(const msg_t &msg) const;

    bool lowrecv(msg_t *out);
    int rawrecv(msg_t *out);

    void finalizeRecv(const msg_t &msg);
    void setEndpoint(const sockaddr_in *addr);
    void setTimeout(size_t newTimeout) const;
    void close();

    int ucpSocket_;
    bool isListenerSocket_;
    sockaddr_in endPoint_;
    msg_buffer_t buffer_;

    uint16_t sendSeqnum_;
    uint16_t recvSeqnum_;

    bool hasEndpoint_;
    sockaddr_in fromEndpoint_;
};

