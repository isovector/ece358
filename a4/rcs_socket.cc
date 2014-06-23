#include "rcs_socket.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sys/select.h>
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

    ucpClose(it->second.ucpSocket_);
    sSocketIdentifiers.erase(it);
}

rcs_t::rcs_t() :
    ucpSocket_(ucpSocket()),
    sendSeqnum_(0),
    recvSeqnum_(0)
{
    //TODO: figure out a sane value for the timeout
    ucpSetSockRecvTimeout(ucpSocket_, 10);
}

int rcs_t::bind(const sockaddr_in *addr) {
    return ucpBind(ucpSocket_, addr);
}

int rcs_t::connect(const sockaddr_in *addr) {
    memcpy(&endPoint_, addr, sizeof(sockaddr_in));
    //TODO: handshake here
    return 0;
}

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

int rcs_t::rawsend(const msg_t &msg) const {
    return ucpSendTo(
        ucpSocket_,
        msg.serialize(),
        static_cast<int>(msg.getTotalLength()),
        &endPoint_
    );
}

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

bool rcs_t::poll() const {
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(ucpSocket_, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    return ucpSelect(1, &rfds, NULL, NULL, &tv) > 0;
}

