#include "Server.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
using namespace std;

//  Description:
//    The constructor identifies a suitable interface to listen on
Server::Server() {
  // Load up all of the network interfaces
  ifaddrs *ifap, *ifap_head;
  getifaddrs(&ifap_head);

  // Necessary to maintain head of linked list so we can delete it later
  ifap = ifap_head;
  do
  {
    // Loop through them, getting their ip addresses and interface names

    if (is_good_interface(ifap))
    {
      // If this is an interface we want, let's use it!
      break;
    }
  } while ((ifap = ifap->ifa_next));

  // Destroy our addresses
  freeifaddrs(ifap_head);
  if (!ifap)
  {
    // Failed to get any sick interfaces. Die in a fire.
    cerr << "error: no good interfaces" << endl;
    exit(1);
  }

  sockaddr_in *sa = (sockaddr_in *) ifap->ifa_addr;
  address_ = inet_ntoa(sa->sin_addr);
}

Server::~Server()
{

}


//  Description:
//    Builds host's connection string
//  Returns:
// 	  string : "<address> <port>"
string Server::get_client_connection_string() const {
  char buffer[16];
  sprintf(buffer, "%d", port_);
  return address_  + " " + buffer;
}

//  Description:
//    Used to filter unusable interfaces
//  Input:
// 	  string name : name of interface being checked
// 	  string ip   : IP address of interface being checked
//  Returns:
// 	  bool : is this a usable interface?
bool Server::is_good_interface(ifaddrs *ifap) {
  if ((ifap->ifa_flags & IFF_LOOPBACK) != 0) {
    // This is a loopback address, not what we want
    return false;
  }

  if (ifap->ifa_addr->sa_family != AF_INET) {
    // This is not an IPv4 address, also not what we want
    return false;
  }

  return true;
}


//  Description:
//    Adds a student to a group.
//  Input:
// 	  int group_id   : group student should be added to
// 	  int student_id : student_id of student being added
// 	  string name    : name of the student to be added
void Server::add_student(int group_id, int student_id, string name)
{
  groups_[group_id][student_id] = name;
}


//  Description:
//    Constructs response to clients' get requests.
//  Input:
// 	  int group_id   : group that requested student is in
// 	  int student_id : id of student whose name is desired by client
string Server::get(int group_id, int student_id)
{
  if (groups_.count(group_id) && groups_[group_id].count(student_id)) {
    return groups_[group_id][student_id];
  }

  stringstream str;
  str << "error: " << group_id << " " << student_id;
  return str.str();
}

#define	PORT_RANGE_LO	10000
#define PORT_RANGE_HI	10100

//  Description:
//    Our implementation of the "mybind.c" code released by the prof
//  Input:
//    int sockfd        : the server's socket file descriptor
//    sockaddr_in *addr : address the server's socket will listen on
//    size_t size       : size of the addr structure
int Server::get_port_and_bind(int sockfd, struct sockaddr_in *addr, size_t size) {
  if(sockfd < 1) {
    fprintf(stderr, "get_port_and_bind(): sockfd has invalid value\n");
    return -1;
  }

  if(addr == NULL) {
    fprintf(stderr, "get_port_and_bind(): addr is NULL\n");
    return -1;
  }

  if(addr->sin_port != 0) {
    fprintf(stderr, "get_port_and_bind(): addr->sin_port is non-zero. Perhaps you want bind() instead?\n");
    return -1;
  }

  unsigned short p;
  for(p = PORT_RANGE_LO; p <= PORT_RANGE_HI; p++) {
    addr->sin_port = htons(p);
    int b = bind(sockfd, (const struct sockaddr *)addr, size);
    if(b < 0) {
      continue;
    }
    else {
      break;
    }
  }

  if(p > PORT_RANGE_HI) {
    fprintf(stderr, "get_port_and_bind(): all bind() attempts failed. No port available...?\n");
    return -1;
  }

  port_ = ntohs(addr->sin_port);
  return 0;
}

