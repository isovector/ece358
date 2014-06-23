#include "rcs_socket.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
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
    ucpSocket_(ucpSocket()),
    sendSeqnum_(0),
    recvSeqnum_(0)
{
    //TODO: figure out a sane value for the timeout
    ucpSetSockRecvTimeout(ucpSocket_, 10);
}

rcs_t::~rcs_t()
{
    ucpClose(ucpSocket_);
}

int rcs_t::bind(const sockaddr_in *addr) {
    int result = ucpBind(ucpSocket_, addr);
    if (result == 0)
        memcpy(&boundAddr_, addr, sizeof(boundAddr_));
    return result;
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

        bool acked = false;
        do {
            rawsend(msg);

            msg_t response;
            if (rawrecv(&response)) {
                if (response.seqnum == sendSeqnum_) {
                    cout << "> ack " << response.seqnum << endl;
                    acked = true;
                    ++sendSeqnum_;
                }
                else
                {
                    cout << "response: " << response.seqnum
                         << "expected: " << sendSeqnum_ << endl;
                }
            }
            else
            {
                // cout << "rawrecv failed" << endl;
            }
        } while (!acked);
    } // end for

    return length;
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
    return out->valid();
}

int rcs_t::recv(char *data, size_t maxLength) {
    while (!buffer_.hasEnoughData(maxLength)) {
        msg_t response;

        if (rawrecv(&response)) {
            if (response.seqnum == recvSeqnum_) {
                cout << "< recv " << response.seqnum << endl;
                buffer_.queueMessage(response);
                rawsend(msg_t(recvSeqnum_, msg_t::MSG_ACK));
                ++recvSeqnum_;
            }
        }
    }

    buffer_.read(data, maxLength);
    return maxLength;
}

sockaddr_in rcs_t::getBoundAddr() const
{
    return boundAddr_;
}

