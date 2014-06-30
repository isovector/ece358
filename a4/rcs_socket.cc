#include "rcs_socket.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sys/select.h>
using namespace std;

static const size_t RECV_TIMEOUT = 10;
static const size_t PROTOCOL_TIMEOUT = 100;

rcs_t::sockets_t rcs_t::sSocketIdentifiers;

//  Description:
//    Static method for retrieving RCS sockets.
//  Input:
//    int id : socket descriptor
//  Output:
//    rcs_t& : RCS socket with the requested id
rcs_t &rcs_t::getSocket(int id) {
    sockets_t::iterator it = sSocketIdentifiers.find(id);
    if (it == sSocketIdentifiers.end()) {
        // make this a proper exception
        throw "BAD SOCKET, SUCKA";
    }

    return it->second;
}

//  Description:
//    Static method for creating RCS sockets
//  Output:
//    int : socket descriptor for the newly created socket
int rcs_t::makeSocket() {
    static size_t sNextId = 0;

    sSocketIdentifiers[sNextId];
    return sNextId++;
}

//  Description:
//    Static method for closing RCS sockets
void rcs_t::destroySocket(int id) {
    sockets_t::iterator it = sSocketIdentifiers.find(id);
    if (it == sSocketIdentifiers.end()) {
        // make this a proper exception
        throw "BAD SOCKET, SUCKA";
    }

    ucpClose(it->second.ucpSocket_);
    sSocketIdentifiers.erase(it);
}

rcs_t::rcs_t() :
    ucpSocket_(ucpSocket()),
    isListenerSocket_(false),
    sendSeqnum_(0),
    recvSeqnum_(0),
    numAccepted_(0),
    hasEndpoint_(false)
{
    //TODO: figure out a sane value for the timeout
    ucpSetSockRecvTimeout(ucpSocket_, 10);
}

//  Description:
//    Implementation of rcsBind
//  Input:
//    const sockaddr_in *addr : the address to which the RCS socket
//                              should be bound to.
//  Output:
//    int : error code
int rcs_t::bind(const sockaddr_in *addr) {
    return ucpBind(ucpSocket_, addr);
}

int rcs_t::getSockName(sockaddr_in *addr) const {
    return ucpGetSockName(ucpSocket_, addr);
}

void rcs_t::listen() {
    isListenerSocket_ = true;
}

int rcs_t::accept(sockaddr_in *addr) {
    if (!isListenerSocket_) {
        return -1;
    }

    sockaddr_in me;
    getSockName(&me);
    ++numAccepted_;
    //TODO: come up with a better way of generating these
    short newPort = ntohs(me.sin_port) + numAccepted_;
    me.sin_port = htons(newPort);


    int childSockfd = makeSocket();
    rcs_t &childSocket = getSocket(childSockfd);
    childSocket.bind(&me);

    char buffer[32];
    setTimeout(0);
    recv(buffer, 32);

    childSocket.setEndpoint(&fromEndpoint_);
    send(
        static_cast<char*>(static_cast<void*>(&newPort)),
        sizeof(short)
    );

    if (addr) {
        memcpy(addr, &fromEndpoint_, sizeof(sockaddr_in));
    }

    return childSockfd;
}

//  Description:
//    Implementation of rcsConnect
//  Input:
//    const sockaddr_in *addr:  address of the server to connect to
//  Output:
//    int : error code
int rcs_t::connect(const sockaddr_in *addr) {
    setEndpoint(addr);

    char connection[16] = "rcs connect";
    send(connection, strlen(connection));

    recv(connection, 16);

    // update our port to what the host told us to use
    endPoint_.sin_port = htons(
        *static_cast<short*>(
            static_cast<void*>(connection)
        )
    );

    setEndpoint(&endPoint_);

    return 0;
}

void rcs_t::setEndpoint(const sockaddr_in *addr) {
    memcpy(&endPoint_, addr, sizeof(sockaddr_in));
    hasEndpoint_ = true;
}

//  Description:
//    Implementation of rcsSend
//  Input:
//    const char *data : buffer containing data to be sent
//    size_t length    : length of data to be sent (in bytes)
//  Output:
//    int : the number of bytes sent
int rcs_t::send(const char *data, size_t length) {
    sendSeqnum_ = 0;

    for (
        size_t processedSize = 0;
        processedSize < length;
        processedSize += MAX_DATA_LENGTH
    ) {
        msg_t msg(
            sendSeqnum_,
            data + processedSize,
            min(length - processedSize, MAX_DATA_LENGTH)
        );

        acksend(msg);
        ++sendSeqnum_;
    }

    msg_t msg(sendSeqnum_, msg_t::EOS);
    acksend(msg);

    return length;
}

//  Description:
//    Used by rcs_t::send to send a message until it has been ACKed
//  Input:
//    const msg_t &msg : the message to be sent
void rcs_t::acksend(const msg_t &msg, msg_t *resp) {
    msg_t response;

    do {
        rawsend(msg);

        msg_t response;
        if (rawrecv(&response) && response.seqnum == msg.seqnum) {
            break;
        }
    } while (true);

    if (resp) {
        memcpy(resp, &response, sizeof(msg_t));
    }
}

//  Description:
//    Wrapper for ucpSendTo
//  Input:
//    msg_t &msg : message to be sent
//  Output:
//    int : error code
int rcs_t::rawsend(const msg_t &msg) const {
    return ucpSendTo(
        ucpSocket_,
        msg.serialize(),
        static_cast<int>(msg.getTotalLength()),
        &endPoint_
    );
}

//  Description:
//    Wrapper for ucpRecvFrom with message deserialization
//  Input:
//    msg_t *out : filled with the message obtained by ucpRecvFrom
//  Output:
//    bool : is the retrieved message valid and of the expected size?
bool rcs_t::rawrecv(msg_t *out) {
    char data[sizeof(msg_t)];

    int result = ucpRecvFrom(
        ucpSocket_,
        static_cast<void*>(data),
        sizeof(msg_t),
        &fromEndpoint_
    );

    if (!hasEndpoint_) {
        setEndpoint(&fromEndpoint_);
    }

    if (result == EWOULDBLOCK || result == EAGAIN) {
        return false;
    }

    *out = msg_t::deserialize(data);
    return out->valid() && static_cast<size_t>(result) == out->getTotalLength();
}

//  Description
//    Implementation of rcsRecv
//  Input:
//    char *data       : buffer to be filled with received data
//    size_t maxLength : maximum amount of data that the buffer can hold
//  Output:
//    int : number of bytes read
int rcs_t::recv(char *data, size_t maxLength) {
    recvSeqnum_ = 0;

    if (buffer_.empty()) {
        while (true) {
            msg_t response;
            msg_t ack(recvSeqnum_ - 1, msg_t::ACK);

            if (rawrecv(&response)) {
                if (response.seqnum == recvSeqnum_) {
                    buffer_.queueMessage(response);
                    ack.seqnum = recvSeqnum_;
                    ++recvSeqnum_;
                }

                rawsend(ack);
                if (response.hasFlag(msg_t::EOS)) {
                    break;
                }
            }
        }

        finalizeRecv();
        ++recvSeqnum_;
    }

    return buffer_.read(data, maxLength);
}

void rcs_t::finalizeRecv() {
    setTimeout(PROTOCOL_TIMEOUT);
    msg_t response;
    while (rawrecv(&response)) {
        if (!response.hasFlag(msg_t::EOS)) {
            break;
        }

        rawsend(msg_t(response.seqnum, msg_t::ACK));
    }
    setTimeout(RECV_TIMEOUT);
}

void rcs_t::setTimeout(size_t newTimeout) const {
    ucpSetSockRecvTimeout(ucpSocket_, static_cast<int>(newTimeout));
}

