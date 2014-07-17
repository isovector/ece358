#include "rcs_t.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sys/select.h>
#include <assert.h>
#include <unistd.h>
using namespace std;

static const size_t     RECV_TIMEOUT = 10;
static const size_t PROTOCOL_TIMEOUT = 500;

static const short PORT_RANGE_LO = 16000;
static const short PORT_RANGE_HI = 16600;

#define INSPECT_PACKETS 0

rcs_t::sockets_t rcs_t::sSocketIdentifiers;


void inspect_packet(int socket, bool send, const msg_t &msg)
{
    if (send) {
        cout << ">>";
    } else {
        cout << "\t<<";
    }

    cout << " " << socket << ":" << msg << "\n";

    if (!send) {
        cout << "\n";
    }
}

//  Description:
//    Static method for retrieving RCS sockets.
//  Input:
//    int id : socket descriptor
//  Output:
//    rcs_t& : RCS socket with the requested id
rcs_t *rcs_t::getSocket(int id)
{
    sockets_t::iterator it = sSocketIdentifiers.find(id);

    if (it == sSocketIdentifiers.end()) {
        return NULL;
    }

    return &it->second;
}

//  Description:
//    Static method for creating RCS sockets
//  Output:
//    int : socket descriptor for the newly created socket
int rcs_t::makeSocket()
{
    static size_t sNextId = 0;

    sSocketIdentifiers[sNextId];
    return sNextId++;
}

//  Description:
//    Static method for closing RCS sockets
void rcs_t::destroySocket(int id)
{
    sockets_t::iterator it = sSocketIdentifiers.find(id);

    if (it == sSocketIdentifiers.end()) {
        return;
    }

    it->second.close();
    sSocketIdentifiers.erase(it);
}

rcs_t::rcs_t() :
    ucpSocket_(ucpSocket()),
    isListenerSocket_(false),
    sendSeqnum_(0),
    recvSeqnum_(0),
    hasEndpoint_(false),
    isClosing_(false)
{
    setTimeout(RECV_TIMEOUT);
}

//  Description:
//    Implementation of rcsBind
//  Input:
//    const sockaddr_in *addr : the address to which the RCS socket
//                              should be bound to.
//  Output:
//    int : error code
int rcs_t::bind(sockaddr_in *addr)
{
    return ucpBind(ucpSocket_, addr);
}

int rcs_t::getSockName(sockaddr_in *addr) const
{
    return ucpGetSockName(ucpSocket_, addr);
}

void rcs_t::listen()
{
    isListenerSocket_ = true;
}

int rcs_t::accept(sockaddr_in *addr)
{
    if (!isListenerSocket_) {
        return -1;
    }

    sockaddr_in me;
    getSockName(&me);
    me.sin_port = 0;

    int childSockfd = makeSocket();
    rcs_t &childSocket = *getSocket(childSockfd);
    childSocket.bind(&me);

    char buffer[32];
    setTimeout(0);

    hasEndpoint_ = false;
    recv(buffer, 32);

    childSocket.setEndpoint(&fromEndpoint_);
    send(
        reinterpret_cast<char *>(&me.sin_port),
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
int rcs_t::connect(const sockaddr_in *addr)
{
    setEndpoint(addr);

    char connection[16] = "rcs connect";
    send(connection, strlen(connection));

    recv(connection, 16);

    // update our port to what the host told us to use
    endPoint_.sin_port = *reinterpret_cast<short *>(connection);
    setEndpoint(&endPoint_);

    return 0;
}

//  Description:
//    Sets the endpoint of a socket
//  Input:
//    const sockaddr_in *addr:  address of the endpoint
void rcs_t::setEndpoint(const sockaddr_in *addr)
{
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
int rcs_t::send(const char *data, size_t length)
{
    sendSeqnum_ = 0;

    for (
        size_t processedSize = 0;
        processedSize < length && !isClosing_;
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

    if (!isClosing_) {
        acksend(msg_t(sendSeqnum_, msg_t::EOS));
    }

    return length;
}

//  Description:
//    Sends a message until it has been ACKED
//  Input:
//    const msg_t &msg : the message to be sent
//    msg_t *resp : filled with the ACK; NULL by default
void rcs_t::acksend(const msg_t &msg, msg_t *resp)
{
    msg_t response;

    do {
        rawsend(msg);

        msg_t response;

        if (lowrecv(&response)) {
            if (response.seqnum == msg.seqnum) {
                break;
            }
        }
    } while (!isClosing_);

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
int rcs_t::rawsend(const msg_t &msg) const
{
#if INSPECT_PACKETS
    inspect_packet(ucpSocket_, true, msg);
#endif

    assert(!(msg.length == 0 && msg.flags == msg_t::NONE));

    return ucpSendTo(
               ucpSocket_,
               msg.serialize(),
               static_cast<int>(msg.getTotalLength()),
               &endPoint_
           );
}

// recv and ensure validity
bool rcs_t::lowrecv(msg_t *out)
{
    int result = rawrecv(out);
    return out->valid() && static_cast<size_t>(result) == out->getTotalLength();
}

// recv something
int rcs_t::rawrecv(msg_t *out)
{
    char data[sizeof(msg_t)] = {0};

    int result = ucpRecvFrom(
                     ucpSocket_,
                     static_cast<void *>(data),
                     sizeof(msg_t),
                     &fromEndpoint_
                 );

    if (!hasEndpoint_) {
        setEndpoint(&fromEndpoint_);
    }

    if (result <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }

        return 1;
    }

    *out = msg_t::deserialize(data);
#if INSPECT_PACKETS
    inspect_packet(ucpSocket_, false, *out);
#endif

    if (out->valid() && static_cast<size_t>(result) == out->getTotalLength()) {
        if (out->hasFlag(msg_t::FIN)) {
            close();
        }
    }

    return result;
}

//  Description
//    Implementation of rcsRecv
//  Input:
//    char *data       : buffer to be filled with received data
//    size_t maxLength : maximum amount of data that the buffer can hold
//  Output:
//    int : number of bytes read
int rcs_t::recv(char *data, size_t maxLength)
{
    do {
        recvSeqnum_ = 0;

        while (!isClosing_) {
            msg_t response;
            msg_t ack(recvSeqnum_ - 1, msg_t::ACK);

            int bytes;

            if ((bytes = rawrecv(&response))) {
                bool isValid = static_cast<size_t>(bytes) == response.getTotalLength()
                               && response.valid();

                if (isValid && response.seqnum == recvSeqnum_) {
                    buffer_.queueMessage(response);
                    ack.seqnum = recvSeqnum_;
                    ++recvSeqnum_;
                }

                if (!isClosing_) {
                    rawsend(ack);
                }

                if (isValid && response.hasFlag(msg_t::EOS)) {
                    finalizeRecv(ack);
                    break;
                }
            }
        }
    } while (buffer_.empty() && !isClosing_);

    if (isClosing_ && buffer_.empty()) {
        return 0;
    }

    return buffer_.read(data, maxLength);
}

//  Description:
//    Ensures proper termination of a send/recv transaction between
//      two parties
//  Input:
//    const msg_t &ack: the ack the sender is expecting
void rcs_t::finalizeRecv(const msg_t &ack)
{
    setTimeout(PROTOCOL_TIMEOUT);
    msg_t response;

    while (rawrecv(&response)) {
        if (response.valid() && response.seqnum == 0 && response.length != 0) {
            break;
        }

        rawsend(ack);
    }

    setTimeout(RECV_TIMEOUT);
}

//  Description:
//    Sets timeout on UCP socket
//  Input:
//    size_t newTimeout: the timeout value in milliseconds. 0 represents infinity
void rcs_t::setTimeout(size_t newTimeout) const
{
    ucpSetSockRecvTimeout(ucpSocket_, static_cast<int>(newTimeout));
}

//  Description:
//    Closes a connection using the most sophisticated
//      techniques available.
void rcs_t::close()
{
    isClosing_ = true;
    // try REALLY hard to close
    for (size_t i = 0; i < 2048; ++i) {
        rawsend(msg_t(666, msg_t::FIN));
    }

    ucpClose(ucpSocket_);
}

