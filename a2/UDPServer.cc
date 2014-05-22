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

//  Description:
//    Creates and binds the server's socket.
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

//  Description:
//    What the server does until it receives a STOP command.
//    Check for incoming messages and send replies.
void UDPServer::run()
{
  char buffer[1024];
  sockaddr_in source_addr;
  socklen_t fromLength = sizeof( source_addr );
  ssize_t recv_msg_size;

  while ( true )
  {
    recv_msg_size = recvfrom(
                 sock_,
                 (void *)buffer,
                 sizeof( buffer ),
                 0,
                 (struct sockaddr *)&source_addr,
                 &fromLength
                );

    if ( recv_msg_size < 0 )
    {
      // Message Failure
      cerr << "error: could not receive message" << endl;
      exit( 1 );
    }

    // process_sending_addr( source_addr );
    // clients_.push_back( source_addr );
    int client_num = get_client_num( source_addr );

    // @TODO check the return value of this function and act accordingly
    bool was_stop_session_msg =  handle_msg( client_num, buffer );

    // if ( was_stop_session_msg )
    // {

    // }
  }
}

//  Description:
//    Reads a message and acts according to the message type.
//  Input:
//    int client         : the client that sent the message
//    const char *reply : the serialized message that was sent to the server
//  Returns:
//    true if the message was of type STOP_SESSION
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

//  Description:
//    Sends a reply to a client.
//  Input:
//    int client      : the client to respond to (index for clients_)
//    string response : the server's response to the client's message
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

//  Description:
//    Process an end of session message from a client
void UDPServer::stop_session( int client )
{

}

//  Description:
//    Finds a client's index in clients_ based on their address.
//  Input:
//    sockaddr_in source_addr : address a message came from
//  Returns:
//    A client's index in clients_
int UDPServer::get_client_num( sockaddr_in source_addr )
{
  sockaddr_in client_addr;
  for ( int i = 0; i < clients_.size(); i++ )
  {
    client_addr = clients_.at(i);
    if ( client_addr.sin_port == source_addr.sin_port &&
         client_addr.sin_addr.s_addr == source_addr.sin_addr.s_addr )
    {
      // Have already communicated with client
      return i;
    }
  }

  clients_.push_back( source_addr );
  return ( clients_.size() - 1 );

}

//  Description:
//    Close down the server.
void UDPServer::stop()
{
  close( sock_ );
  exit( 1 );
}
