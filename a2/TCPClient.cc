#include "TCPClient.h"

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

//  Description:
//    The constructor creates the client's TCP socket
TCPClient::TCPClient()
    : sock_(-1)
{
    sock_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock_ == -1) {
        cerr << "error: unable to create socket" << endl;
        exit(1);
    }

    memset(&addr_, 0, sizeof(sockaddr_in));
    addr_.sin_family = AF_INET;
    // @TODO(Ariel): Determine a usable port number on ecelinux
    addr_.sin_family = htons(0);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);

    // Connection to host set in set_host_info
}

//  Description:
//    The destructor ensures that the socket is closed correctly
TCPClient::~TCPClient()
{
    (void) shutdown(sock_, SHUT_RDWR);

    close(sock_);

}

//  Description:
//    Set host address info based on input from stdin.
//    Will convert from domain name to IP address if necessary
//  Input:
//    string host_address : the server's address, either a domain name or IP address
//    string host_port    : the port that the server is listening on
void TCPClient::set_host_info(string host_address, string host_port)
{
    host_port_ = atoi(host_port.c_str());

    addrinfo *lookup, *lookup_head;
    // Determine our endpoint for the socket
    int result = getaddrinfo(
                     host_address.c_str(),
                     host_port.c_str(),
                     NULL,
                     &lookup_head);

    if (result < 0) {
        // name lookup failed
        cerr << "error: could not find host: " << host_address << endl;
        exit(1);
    }

    // search through all of the resolved addresses
    lookup = lookup_head;

    while (lookup) {
        if (lookup->ai_family != AF_INET) {
            lookup = lookup->ai_next;
            continue;
        }

        memcpy(&host_addr_, lookup->ai_addr, sizeof(sockaddr_in));
        break;
    }

    if (!lookup) {
        // name resolution failed
        cerr << "error: could not find host: " << host_address << endl;
        exit(1);
    }

    freeaddrinfo(lookup_head);

    result = connect(sock_, (struct sockaddr *)&host_addr_, sizeof(host_addr_));

    if (result == -1) {
        cerr << "connection failed (errno: " << errno << ")" << endl;
        close(sock_);
        exit(1);
    }
}

//  Description:
//    Sends a message to the server and receives reply
//  Input:
//    Message msg : the message to be sent to the server
//  Output:
//    char buffer[BUFFER_SIZE] : the server's reply
string TCPClient::send_message(Message msg)
{
    int result = send(sock_, msg.serialize(), sizeof(Message), 0);

    if (result != sizeof(Message)) {
        cerr << "error: failed to sent entire message" << endl;
        exit(1);
    }

    static const size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    ssize_t bytes = sizeof(sockaddr_in);
    result = recv(
                 sock_,
                 buffer,
                 BUFFER_SIZE,
                 0
             );

    if (bytes <= 0) {
        cerr << "error: bad reply from server" << endl;
        exit(1);
    }

    buffer[result] = '\0';

    return buffer;

}
