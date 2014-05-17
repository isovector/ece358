#include "UDPServer.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Message.h"


using namespace std;

UDPServer::UDPServer()
{

}

UDPServer::~UDPServer()
{

}

void UDPServer::init()
{
  sock_ = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

  if ( sock_ == -1 )
  {
    cerr << "error: unable to listen" << endl;
    close( sock_ );
    exit( 1 );
  }

  // Init address for binding
  memset( &socket_address_, 0, sizeof( socket_address_ ));
  socket_address_.sin_family = AF_INET;

  int result;
  result = inet_pton( AF_INET, address_.c_str(), &socket_address_.sin_addr );

  if ( result < 0 )
  {
    // Address conversion failed
    cerr << "error: first parameter is not a valid address family" << endl;
    close( sock_ );
    exit( 1 );
  }
  else if ( result == 0 )
  {
    // Address conversion failed
    cerr << "error: second parameter is not a valid IP address" << endl;
    close( sock_ );
    exit( 1 );
  }

  // Attempt to bind
  result = get_port_and_bind( sock_, &socket_address_, sizeof( socket_address_ ) );

  if ( result == -1 )
  {
    // Bind failed
    cerr << "error: unable to bind to address" << endl;
    close( sock_ );
    exit( 1 );
  }
}

void UDPServer::run()
{
  char buffer[1024];
  sockaddr_in sending_addr;
  socklen_t fromLength = sizeof( sending_addr );
  ssize_t recv_msg_size;

  while ( true )
  {
    cout << "Waiting for message..." << endl;
    recv_msg_size = recvfrom(
                 sock_,
                 (void *)buffer,
                 sizeof( buffer ),
                 0,
                 (struct sockaddr *)&sending_addr,
                 &fromLength
                );
    cout << "Message received..." << endl;

    if ( recv_msg_size < 0 )
    {
      // Message Failure
      cerr << "error: could not receive message" << endl;
      exit( 1 );
    }

    // process_sending_addr( sending_addr );
    clients_.push_back( sending_addr );

    handle_msg( 0, buffer );

    cout << "recv_msg_size: " << recv_msg_size << endl;
    cout << "datagram: " << buffer << endl;

  }
}

/*
  Handles a message
*/
bool UDPServer::handle_msg(int client, const char *reply)
{
  Message msg = Message::deserialize(reply);
  switch (msg.command) {
    case Message::GET:
      respond(client, get( msg.param1, msg.param2 ));
      return true;

    case Message::STOP_SESSION:
      // We intentionally do not call stop_session() due to iterator woes -
      // this path actually executes in run()
      return false;

    case Message::STOP:
      stop();
      return true;

    default:
      cerr << "error: invalid command (corrupted message?)" << endl;
      return true;
  }
}

/*
  Responds to a client
  input:
    int client: handle on which client to reply to
    string reply: the reply to send
*/
void UDPServer::respond(int client, string reply)
{
  sendto(
    sock_,
    reply.c_str(),
    reply.length(),
    0,
    (struct sockaddr*)&clients_.at( client ),
    sizeof( clients_.at( client ) )
  );
}

/*
  Process an end of session message from a client
*/
void UDPServer::stop_session( int client )
{

}

/*
  Server Termination
*/
void UDPServer::stop()
{
}
