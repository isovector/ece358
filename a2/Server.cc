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

  address_ = ip;
}

string Server::get_client_connection_string() const {
  char buffer[16];
  sprintf(buffer, "%d", port_);
  return address_  + ":" + buffer;
}

bool Server::is_good_interface(std::string name, std::string ip) {
  if (name != "wlan0" && name != "eth0") {
    return false;
  }

  return ip.substr(0, 3) == "10.";
}

void Server::add_student(int group_id, int student_id, string name)
{
  groups_[group_id][student_id] = name;
}

string Server::get(int group_id, int student_id) {
  if (groups_.count(group_id) && groups_[group_id].count(student_id)) {
    return groups_[group_id][student_id];
  }

  stringstream str;
  str << "error: " << group_id << " " << student_id;
  return str.str();
}

int Server::get_port() const {
  // TODO: figure out how to do this properly
  return 1337;
}

