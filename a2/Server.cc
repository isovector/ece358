#include "Server.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
using namespace std;

Server::Server() {
  char *ip;

  // Load up all of the network interfaces
  ifaddrs *ifap, *ifap_head;
  getifaddrs(&ifap_head);

  // Necessary to maintain head of linked list so we can delete it later
  ifap = ifap_head;
  do
  {
    // Loop through them, getting their ip addresses and interface names
    sockaddr_in *sa = (sockaddr_in *) ifap->ifa_addr;
    ip = inet_ntoa(sa->sin_addr);

    if (is_good_interface(ifap->ifa_name, ip))
    {
      // If this is an interface we want, let's use it!
      break;
    }
  } while ((ifap = ifap->ifa_next));

  freeifaddrs(ifap_head);
  if (!ifap)
  {
    // Failed to get any sick interfaces. Die in a fire.
    cerr << "error: no good interfaces" << endl;
    exit(1);
  }

  address_ = ip;
}

Server::~Server()
{

}

/*
	Builds host's connection string
	Returns:
		string - "<address>:<port>"
*/
string Server::get_client_connection_string() const {
  char buffer[16];
  sprintf(buffer, "%d", port_);
  return address_  + ":" + buffer;
}

/*
	Used to filter unusable interfaces
	Input:
		string name - name of interface being checked
		string ip   - IP address of interface being checked
	Returns:
		bool - is this a usable interface?
*/
bool Server::is_good_interface(string name, string ip) {
  if (name != "wlan0" && name != "eth0") {
    return false;
  }

  return ip.substr(0, 3) == "10.";
}

/*
	Adds a student to a group
	Input:
		int group_id   - group student should be added to
		int student_id - student_id of student being added
		string name    - name of the student to be added
*/
void Server::add_student(int group_id, int student_id, string name)
{
  groups_[group_id][student_id] = name;
}

/*
	Constructs response to clients' get requests
	Input:
		int group_id   : group that requested student is in
		int student_id : id of student whose name is desired by client
*/
string Server::get(int group_id, int student_id)
{
  if (groups_.count(group_id) && groups_[group_id].count(student_id)) {
    return groups_[group_id][student_id];
  }

  stringstream str;
  str << "error: " << group_id << " " << student_id;
  return str.str();
}

/*
	Finds an available port
	Returns:
		int - available port number
*/
int Server::get_port() const
{
  // TODO: figure out how to do this properly
  return 1337;
}

