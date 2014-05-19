#include "TCPClient.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

TCPClient::TCPClient()
  : sock_(-1)
{
  sock_ = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

  if ( sock_ == -1 )
  {
    cerr << "error: unable to create socket" << endl;
    exit( 1 );
  }

  int result;
  memset( &addr_, 0, sizeof( sockaddr_in ) );
  addr_.sin_family = AF_INET;
  // @TODO(Ariel): Determine a usable port number on ecelinux
  addr_.sin_family = htons( 0 );
  addr_.sin_addr.s_addr = htonl( INADDR_ANY );

  // Connection to host set in set_host_info
}

TCPClient::~TCPClient()
{
   (void) shutdown( sock_, SHUT_RDWR );

   close( sock_ );

}

/*
  Initialize socket connection

*/
void TCPClient::set_host_info( string host_address, int host_port )
{
  host_port_ = host_port;

  int result;

  memset( &host_addr_, 0, sizeof( host_addr_ ) );
  host_addr_.sin_family = AF_INET;
  host_addr_.sin_port = htons( host_port_ );
  result = inet_pton( AF_INET, host_address.c_str(), &host_addr_.sin_addr );

  if ( result < 0 )
  {
    cerr << "First parameter is not a valid address family" << endl;
    close( sock_ );
    exit( 1 );
  }
  else if ( result == 0 )
  {
    cerr << "Second paramter is not a valid IP address" << endl;
    close( sock_ );
    exit( 1 );
  }

  result = connect( sock_, (struct sockaddr *)&host_addr_, sizeof( host_addr_ ) );

  if ( result == -1 )
  {
    cerr << "connection failed" << endl;
    close( sock_ );
    exit( 1 );
  }

}

string TCPClient::send_message( Message msg )
{
  int result = send( sock_, msg.serialize(), sizeof( Message ), 0 );

  if ( result != sizeof( Message ) )
  {
    cerr << "error: failed to sent entire message" << endl;
    exit( 1 );
  }

  static const size_t BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  cout << "Waiting for message" << endl;

  socklen_t addrLength = sizeof( sockaddr_in );
  result = recvfrom(
    sock_,
    buffer,
    BUFFER_SIZE,
    0,
    reinterpret_cast<sockaddr*>(&host_addr_),
    &addrLength
  );


  buffer[result] = '\0';

  cout << "Received message: " << endl;

  return buffer;

}