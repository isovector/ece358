#include "TCPClient.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>


using namespace std;

TCPClient::TCPClient()
{

}

TCPClient::~TCPClient()
{

}

/*
	Initialize socket connection

*/
void TCPClient::set_host_info( string host_address, int host_port )
{
	host_port_ = host_port;
	sock_ = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( sock_ == -1 )
	{
		cerr << "error: unable to create socket" << endl;
		exit( 1 );
	}

	// memset( socket_address_, 0, sizeof( socket_address_ ) );
	// socket_address_.sin_family = AF_INET;
	// socket_address_.sin_port = htons( host_port_ );

	// int result;
	// // Expects an IP address XXX.XXX.XXX.XXX
	// // Would need to convert a hostname to IP
	// result = inet_pton( AF_INET, host_address, &socket_address_.sin_addr );
 //    stSockAddr.sin_family = AF_INET;
 //    stSockAddr.sin_port = htons(1100);
 //    Res = inet_pton(AF_INET, "192.168.1.3", &stSockAddr.sin_addr);

 //    if (0 > Res)
 //    {
 //      perror("error: first parameter is not a valid address family");
 //      close(SocketFD);
 //      exit(EXIT_FAILURE);
 //    }
 //    else if (0 == Res)
 //    {
 //      perror("char string (second parameter does not contain valid ipaddress)");
 //      close(SocketFD);
 //      exit(EXIT_FAILURE);
 //    }

 //    if (-1 == connect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)))
 //    {
 //      perror("connect failed");
 //      close(SocketFD);
 //      exit(EXIT_FAILURE);
 //    }

 //    /* perform read write operations ... */

 //    (void) shutdown(SocketFD, SHUT_RDWR);

 //    close(SocketFD);
 //    return EXIT_SUCCESS;
}

string TCPClient::send_message( Message msg )
{

}