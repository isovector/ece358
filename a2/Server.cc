#include "Server.h"
#include <sstream>
using namespace std;


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

