#include "UDPServer.h"

using namespace std;

UDPServer::UDPServer()
{

}

UDPServer::~UDPServer()
{

}

void UDPServer::run()
{

}

void UDPServer::add_student(int group_id, int student_id, string name)
{

}

bool UDPServer::handle_msg(int client, const char *reply)
{

}

void UDPServer::respond(int client, string reply)
{

}

// STOP SESSION: signals the Server that the client will stop sending data.
void UDPServer::stop_session()
{

}

// STOP: tells the Server to terminate; i.e., the Server process dies. Termination must be graceful.
// STOP implies STOP SESSION.
void UDPServer::stop()
{
}
