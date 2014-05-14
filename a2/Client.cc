#include "Client.h"
#include <cstdlib>
#include <ifaddrs.h>
#include <iostream>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

Client::Client()
{
  char *ip;

  // Load up all of the network interfaces
  ifaddrs *ifap, *ifap_head;
  getifaddrs(&ifap_head);

  // Necessary to maintain head of linked list so we can delete it later
  ifap = ifap_head;
  do {
    // Loop through them, getting their ip addresses and interface names
    sockaddr_in *sa = (sockaddr_in *) ifap->ifa_addr;
    ip = inet_ntoa(sa->sin_addr);

    if (is_good_interface(ifap->ifa_name, ip)) {
      // If this is an interface we want, let's use it!
      break;
    }
  } while ((ifap = ifap->ifa_next));

  freeifaddrs(ifap_head);
  if (!ifap) {
    // Failed to get any sick interfaces. Die in a fire.
    cerr << "error: no good interfaces" << endl;
    exit(1);
  }
  else {
    // TODO(sandy): get rid of this
    cout << "using " << ip << endl;
  }

  host_address_ = ip;
}

Client::~Client()
{

}

bool Client::is_good_interface(std::string name, std::string ip) {
  if (name != "wlan0" && name != "eth0") {
    return false;
  }

  return ip.substr(0, 3) == "10.";
}

// The client program sends GET commands to the server to
// retrieve students names based on their group number and id w/in the group
// It reads which group and student ID it should perform a GET for by reading
// from stdin. It prints non-error responses it receives from
// the server to stdout. It prints error responses it receives to stderr.
void Client::send_get( int group_id, int student_id )
{
  send_message(Message(
    Message::GET, 
    static_cast<uint32_t>(group_id), 
    static_cast<uint32_t>(student_id)
  ));
}

// If the client receives an EOF on stdin, it sends
//	a STOP SESSION command to the server, and terminates itself.
// If the client reads the special string STOP on stdin, it sends
//  the STOP command to the server, and terminates itself.
// If one (any) client sends a STOP command, this causes the server
//  it is communicating with, to terminate gracefully.
void Client::send_stop_session()
{
  send_message(Message::STOP_SESSION);
}

void Client::send_stop()
{
  send_message(Message::STOP);
}

