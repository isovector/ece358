#include "UDPClient.h"
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

//  Description:
//    The constructor creates and binds the client's socket
UDPClient::UDPClient()
  : sock_(-1)
{
  // Create a UDP socket
  sock_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock_ == -1)
  {
    cerr << "error: unable to create socket" << endl;
    exit(1);
  }

  int result;

  // Determine our endpoint for the socket
  memset( &addr_, 0, sizeof( sockaddr_in ) );
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons( 0 );
  addr_.sin_addr.s_addr = htonl( INADDR_ANY );

  // Bind and listen to the socket
  result = bind(sock_, reinterpret_cast<sockaddr*>(&addr_), sizeof(sockaddr_in));
  if (result < 0)
  {
    cerr << "error: unable to bind to address " << errno << endl;
    exit(1);
  }
}

//  Description:
//    The destructor closes the socket if necessary.
UDPClient::~UDPClient()
{
  if (sock_ != -1) {
    close(sock_);
  }
}

//  Description:
//    Sends a message to the server
//  Input:
//    Message msg : the message to be sent to the server
//  Returns:
//    char buffer[BUFFER_SIZE] : the server's reply
string UDPClient::send_message(Message msg)
{
  // Send the message to sock_
  int result = sendto(
    sock_,
    msg.serialize(),
    sizeof(Message),
    0,
    reinterpret_cast<sockaddr*>(&host_addr_),
    sizeof(sockaddr_in)
  );

  if (result != sizeof(Message))
  {
    // We didn't send as many bytes as we expected. Die in a fire!
    cerr << "error: failed to send entire message" << endl;
    exit(1);
  }

  static const size_t BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  // Block until we have received a response
  socklen_t addrLength = sizeof(sockaddr_in);
  result = recvfrom(
    sock_,
    buffer,
    BUFFER_SIZE,
    0,
    reinterpret_cast<sockaddr*>(&host_addr_),
    &addrLength
  );

  buffer[result] = '\0';
  return buffer;
}

//  Description:
//    Set host address info based on input from stdin.
//    Will convert from domain name to IP address if necessary
//  Input:
//    string host_address : the server's address, either a domain name or IP address
//    string host_port    : the port that the server is listening on
void UDPClient::set_host_info( string host_address, string host_port )
{
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

  // loop through resolved addresses, finding one that is appropriate
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

  host_addr_.sin_family = AF_INET;
  host_addr_.sin_port = htons( atoi(host_port.c_str()) );
}

