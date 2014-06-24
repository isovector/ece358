#include "rcs_socket.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sys/select.h>
using namespace std;

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
    sendSeqnum_(0),
    recvSeqnum_(0),
    isListenerSocket_(false)
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

//  Description:
//    Implementation of rcsConnect
//  Input:
//    const sockaddr_in *addr:  address of the server to connect to
//  Output:
//    int : error code
int rcs_t::connect(const sockaddr_in *addr) {
    memcpy(&endPoint_, addr, sizeof(sockaddr_in));
    return 0;
}

//  Description:
//    Implementation of rcsSend
//  Input:
//    const char *data : buffer containing data to be sent
//    size_t length    : length of data to be sent (in bytes)
//  Output:
//    int : the number of bytes sent
int rcs_t::send(const char *data, size_t length) {
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
void rcs_t::acksend(const msg_t &msg) const {
    bool acked = false;
    do {
        rawsend(msg);

        msg_t response;
        if (rawrecv(&response) && response.seqnum == msg.seqnum) {
            break;
        }
    } while (!acked);
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
bool rcs_t::rawrecv(msg_t *out) const {
    char data[sizeof(msg_t)];
    sockaddr_in unused;

    int result = ucpRecvFrom(
        ucpSocket_,
        static_cast<void*>(data),
        sizeof(msg_t),
        &unused
    );

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
    if (buffer_.empty() || poll()) {
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

        while (poll()) {
            msg_t response;
            if (rawrecv(&response)) {
                if (response.hasFlag(msg_t::EOS)) {
                    rawsend(msg_t(response.seqnum, msg_t::ACK));
                } else {
                    break;
                }
            }
        }

        ++recvSeqnum_;
    }

    return buffer_.read(data, maxLength);
}

//  Description:
//    Getter for an RCS socket's underlying UCP socket
//  Output:
//    int : file descriptor of the UCP socket
int rcs_t::getUcpSocket() const {
    return ucpSocket_;
}

//  Description:
//    Marks an RCS socket as a listener socket
//    (One that accepts connection requests)
void rcs_t::markAsListenerSocket() {
    isListenerSocket_ = true;
}

//  Description:
//    Getter for isListenerSocket_ marker
//  Output:
//    bool : is this socket a listener socket?
bool rcs_t::isListenerSocket() const {
    return isListenerSocket_;
}

bool rcs_t::poll() const {
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(ucpSocket_, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    return ucpSelect(1, &rfds, NULL, NULL, &tv) > 0;
}

