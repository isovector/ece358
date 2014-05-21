#include "Client.h"
#include <cstdlib>
#include <iostream>
#include <stdint.h>
using namespace std;

Client::Client()
{
}

Client::~Client()
{

}

//  Description:
//    Sends a GET message to the server and prints the reply.
//  Input:
//    int group_id   : the id of the group the student is a part of
//    int student_id : the id of the student we're interested in
void Client::send_get( int group_id, int student_id )
{
  string reply = send_message( Message(
                  Message::GET,
                  static_cast<uint32_t>(group_id),
                  static_cast<uint32_t>(student_id)
                 ));
  cout << reply << endl;
}

//  Description:
//    Sends a STOP_SESSION message to the server.
void Client::send_stop_session()
{
  send_message(Message::STOP_SESSION);
}

//  Description:
//    Sends a STOP message to the server
void Client::send_stop()
{
  send_message(Message::STOP);
}

