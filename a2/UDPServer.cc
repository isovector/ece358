#include "UDPServer.h"

UDPServer::UDPServer()
{

}

UDPServer::~UDPServer()
{

}

// GET <GROUP ID> <STUDENT ID>: returns the name of the student with id STUDENT ID in the
// group numbered GROUP ID.
// In case of an invalid group or student id, the text ERROR <GROUP ID> <STUDENT ID> where
// <GROUP ID> is replaced by the group number and <STUDENT ID> by the id of the student within
// the group, is sent back to the client.
void UDPServer::get( int group_id, int student_id )
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
