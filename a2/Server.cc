#include "Server.h"
using namespace std;


void Server::add_student(int group_id, int student_id, string name)
{
  groups_[group_id][student_id] = name;
}

int Server::get_port() const {
  // TODO: figure out how to do this properly
  return 1337;
}

