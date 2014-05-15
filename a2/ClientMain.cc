#ifdef TCP
#include "TCPClient.h"
TCPClient client;
#else
#include "UDPClient.h"
UDPClient client;
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

// The client is provided the following two command-line arguments, in order.
//		(1) The IP address or domain name of the server.
//		 	Your client should be able to handle both.
//		(2) The port number on which the server runs.
int main( int argc, char *argv[] )
{
	// Ensure command line arguments are provided
	if ( argc != 3 )
	{
		cerr << "error: invalid number of command line arguments" << endl;
		exit(0);
	}
  client.set_host_info( argv[1], atoi( argv[2] ) );

  char buffer[1024];
  while (cin.getline(buffer, 1024)) {
    stringstream str;
    string command;

    str << buffer;
    str >> command;
    if (command == "GET")
    {
      int group_id, student_id;

      str >> group_id >> student_id;
      if (!str || group_id < 0 || student_id < 0)
      {
        // An error occurred. We're not sure what, but we handle them all
        // the same way.
        //
        // Since we have already read from the strstream, we can't get the
        // information out of it anymore, so shift our original buffer
        // pointer by the length of the prefix ("GET")
        //
        // It's kind of hacky, but at least it's well documented, right?
        cerr << "error: " << (buffer + sizeof("GET")) << endl;
        continue;
      }

      client.send_get(group_id, student_id);
    }
    else if (command == "STOP")
    {
      client.send_stop();
    }
    else
    {
      // An invalid command
      cerr << "error: invalid input" << endl;
    }
  }

	client.send_stop_session();
}

