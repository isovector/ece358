#include "UDPServer.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
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
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_address.sin_port = htons( get_port() );

	// Attempt to bind
	int result;
	result = bind( sock, (struct sockaddr *)&socket_address, sizeof( socket_address ) );

	if ( result == -1 )
	{
		// Bind failed
		cerr << "error: unable to bind to address" << endl;
		close( sock );
		exit( 1 );
	}

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
void UDPServer::stop_session()
{

}

/*
	Server Termination
*/
void UDPServer::stop()
{
}
