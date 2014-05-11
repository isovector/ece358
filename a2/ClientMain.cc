#ifdef TCP
#include "TCPClient.h"
TCPClient client;
#else
#include "UDPClient.h"
UDPClient client;
#endif

// The client is provided the following two command-line arguments, in order.
//		(1) The IP address or domain name of the server.
//		 	Your client should be able to handle both.
//		(2) The port number on which the server runs.
int main( int argc, char *argv[] )
{

}

