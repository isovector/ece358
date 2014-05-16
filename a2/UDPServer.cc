#include "UDPServer.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
// #include "Message.h"


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
	socklen_t fromLength = sizeof(socket_address_);
	ssize_t recv_msg_size;

	while ( true )
	{
		recv_msg_size = recvfrom( sock_, (void *)buffer, sizeof( buffer ), 0, (struct sockaddr *)&socket_address_, &fromLength );

		if ( recv_msg_size < 0 )
		{
			// Message Failure
			cerr << "error: could not receive message" << endl;
			exit( 1 );
		}

		cout << "recv_msg_size: " << recv_msg_size << endl;
		cout << "datagram: " << buffer << endl;

	}
}

/*
	Handles a message
*/
bool UDPServer::handle_msg(int client, const char *reply)
{
  return true;
}

/*
	Responds to a client
	input:
		int client: handle on which client to reply to
		string reply: the reply to send
*/
void UDPServer::respond(int client, string reply)
{

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
