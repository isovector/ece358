#include "UDPClient.h"
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
using namespace std;

UDPClient::UDPClient()
  : sock_(-1)
{

}

UDPClient::~UDPClient()
{
  if (sock_ != -1) {
    close(sock_);
  }
}

string UDPClient::send_message(Message msg)
{
  int result = sendto(
    sock_, 
    msg.serialize(), 
    sizeof(Message), 
    0, 
    reinterpret_cast<sockaddr*>(&addr_), 
    sizeof(sockaddr_in)
  );

  if (result != sizeof(Message)) {
    cerr << "error: failed to send entire message" << endl;
    exit(1);
  }

  static const size_t BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  socklen_t addrLength = sizeof(sockaddr_in);
  result = recvfrom(
    sock_, 
    buffer, 
    BUFFER_SIZE, 
    0, 
    reinterpret_cast<sockaddr*>(&addr_), 
    &addrLength
  );

  buffer[result] = '\0';
  return buffer;
}

void UDPClient::set_host_info( string host_address, int host_port )
{
  sock_ = socket(PF_INET, SOCK_DGRAM, 0);

  if (sock_ == -1) {
    cerr << "error: unable to create socket" << endl;
    exit(1);
  }

  // Determine our endpoint for the socket
  // TODO(sandy): figure out how to do this properly
  memset(&addr_, 0, sizeof(sockaddr_in));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(host_port);
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind and listen to the socket
  int result;
  result = bind(sock_, reinterpret_cast<sockaddr*>(&addr_), sizeof(sockaddr_in));
  if (result < 0) {
    cerr << "error: unable to bind to address " << errno << endl;
    close(sock_);
    exit(1);
  }
}

