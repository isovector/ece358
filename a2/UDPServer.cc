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
	int sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( sock == -1 )
	{
		cerr << "error: unable to listen" << endl;
		close( sock );
		exit( 1 );
	}

	// Init address for binding
	sockaddr_in socket_address;
	memset( &socket_address, 0, sizeof( socket_address ));
	socket_address.sin_family = AF_INET;

	int result;
	result = inet_pton( AF_INET, address_.c_str(), &socket_address.sin_addr );

	if ( result < 0 )
	{
		// Address conversion failed
		cerr << "error: first parameter is not a valid address family" << endl;
		close( sock );
		exit( 1 );
	}
	else if ( result == 0 )
	{
		// Address conversion failed
		cerr << "error: second parameter is not a valid IP address" << endl;
		close( sock );
		exit( 1 );
	}

	// Attempt to bind
	result = get_port_and_bind( sock, &socket_address, sizeof( socket_address ) );

	if ( result == -1 )
	{
		// Bind failed
		cerr << "error: unable to bind to address" << endl;
		close( sock );
		exit( 1 );
	}

	cout << "Listening on " << address_ << ":" << port_ << endl;
}

void UDPServer::run()
{
}

/*
	Handles a message
*/
bool UDPServer::handle_msg(int client, const char *reply)
{

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
