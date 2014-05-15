#include "UDPClient.h"
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
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
  // Send the message to sock_
  int result = sendto(
    sock_, 
    msg.serialize(), 
    sizeof(Message), 
    0, 
    reinterpret_cast<sockaddr*>(&addr_), 
    sizeof(sockaddr_in)
  );

  if (result != sizeof(Message)) {
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
    reinterpret_cast<sockaddr*>(&addr_), 
    &addrLength
  );

  buffer[result] = '\0';
  return buffer;
}

void UDPClient::set_host_info( string host_address, int host_port )
{
  // Create a UDP socket
  sock_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock_ == -1) {
    cerr << "error: unable to create socket" << endl;
    exit(1);
  }

  int result;

  // Determine our endpoint for the socket
  memset(&addr_, 0, sizeof(sockaddr_in));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(host_port);
  result = inet_pton(AF_INET, host_address.c_str(), &addr_.sin_addr.s_addr);
  if (result == 0) {
    // inet_pton returns 0 for failed hostname
    cerr << "error: bad host_address given: " << host_address << endl;
    exit(1);
  }

  // Bind and listen to the socket
  result = bind(sock_, reinterpret_cast<sockaddr*>(&addr_), sizeof(sockaddr_in));
  if (result < 0) {
    cerr << "error: unable to bind to address " << errno << endl;
    exit(1);
  }
}

