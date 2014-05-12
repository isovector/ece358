#include "Client.h"

Client::Client()
{

}

Client::~Client()
{

}

// The client program sends GET commands to the server to
// retrieve students names based on their group number and id w/in the group
// It reads which group and student ID it should perform a GET for by reading
// from stdin. It prints non-error responses it receives from
// the server to stdout. It prints error responses it receives to stderr.
void Client::send_get( int group_id, int student_id )
{

}

// If the client receives an EOF on stdin, it sends
//	a STOP SESSION command to the server, and terminates itself.
// If the client reads the special string STOP on stdin, it sends
//  the STOP command to the server, and terminates itself.
// If one (any) client sends a STOP command, this causes the server
//  it is communicating with, to terminate gracefully.
void Client::send_stop_session()
{

}
